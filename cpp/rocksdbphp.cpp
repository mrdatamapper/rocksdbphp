/**
 *  rocksdbphp.cpp
 *  app RocksDB
 *  implementation fast key/value storage RocksDB for php
 *  @author valmat <ufabiz@gmail.com>
 *  @github https://github.com/valmat/rocksdbphp
 */



#include <string>
#include <iostream>
#include <phpcpp.h>
#include "rocksdb/db.h"
#include "rocksdb/write_batch.h"
#include "rocksdb/merge_operator.h"

//#include <php5/Zend/zend_iterators.h>
/**
 *  PHP includes
 */
//#include <php.h>
//#include "zend_iterators.h"



//using namespace std;

namespace RocksDBPHP {
	class Int64Incrementor : public rocksdb::AssociativeMergeOperator {
	 public:
	    virtual bool Merge(
	        const rocksdb::Slice& key,
	        const rocksdb::Slice* existing_value,
	        const rocksdb::Slice& value,
	        std::string* new_value,
	        rocksdb::Logger* logger) const override {

	        // assuming 0 if no existing value
	        int64_t existing = 0;
	        if (existing_value) {
	             existing = std::strtoull(existing_value->ToString().c_str(), nullptr, 10);
	        }

	        int64_t incr = std::strtoull(value.ToString().c_str(),0,10);
	        int64_t newval = existing + incr;
	        *new_value = std::to_string(newval);
	        return true;        // always return true for this, since we treat all errors as "zero".
	    }

	    virtual const char* Name() const override {
	        return "Int64Incrementor";
	    }
	};
//}

//namespace RocksDBPHP {
	class Driver : public Php::Base//, public zend_object_iterator
	{
	private:
	    /**
	     * db
	     */
	    rocksdb::DB* db;

	    /**
	     * db path distantion
	     */
	    std::string dbpath;

	    /**
	     * db path distantion
	     */
	    rocksdb::Options dboptions;

	    /**
	     * last opreation status
	     */
	    rocksdb::Status status;
    


	public:

	    Driver() = default;// noexcept

	    /**
	     * function __construct
	     * @param string path
	     * @param bool create_if_missing
	     */
	    virtual void __construct(Php::Parameters &params) {

	        if (params.size() < 1) {
	            throw Php::Exception("Requires parameter path");
	            return;
	        }
	        this->dbpath = params[0].stringValue();
	        bool create_if_missing = true;
	        if (params.size() > 1) {
	            create_if_missing = (bool)params[1];
	        }

	        this->dboptions.create_if_missing = create_if_missing;
	        this->dboptions.merge_operator.reset(new Int64Incrementor);
	        this->status = rocksdb::DB::Open(this->dboptions, this->dbpath, &this->db);
	        std::cerr << this->status.ToString() << std::endl;

	        if (!this->status.ok())  {
	            throw Php::Exception(   this->status.ToString()  );
	        }
	    }

	    virtual ~Driver() {
	        //delete Incrementor; //not required : std::shared_ptr
	        delete db;
	    }
	    virtual void __destruct() {}

	    /**
	     * set value by key
	     * function set
	     * @param string key
	     * @param string value
	     * @return bool rezult
	     */
	    Php::Value set(Php::Parameters &params) {
	        if (params.size() < 2) {
	            throw Php::Exception("Requires 2 parameters: key and value");
	            return false;
	        }
	        std::string key, value;
	        key   = params[0].stringValue();
	        value = params[1].stringValue();
	        //std::cout << "set("<< key << ")=" << value << std::endl;

	        this->status = db->Put(rocksdb::WriteOptions(), key, value);
	        return static_cast<bool>(this->status.ok());
	    }

	    /**
	     * set array values by array keys
	     * function mset
	     * @param Php::Array keys
	     * @param Php::Array values
	     * @return bool rezult status
	     */
	    Php::Value mset(Php::Parameters &params) {
	        if (params.size() < 2) {
	            throw Php::Exception("Requires 2 parameter: array keys and array values");
	            return false;
	        }
	        if(!params[0].isArray() || !params[1].isArray() ) {
	            throw Php::Exception("Required parameters are arrays");
	            return false;
	        }
	        if(params[0].size() != params[1].size() ) {
	            throw Php::Exception("Dimensions do not match");
	            return false;
	        }

	        unsigned int arrSize = params[0].size();
	        rocksdb::WriteBatch batch;
	        std::string key, val;

	        for (unsigned int i = 0; i < arrSize; i++) {
	            key = params[0][i].value().stringValue();
	            val = params[1][i].value().stringValue();
	            
	            std::cout << "set("<< key << ")=" << val << std::endl;
	            batch.Put(key, val);
	        }
	        this->status = db->Write(rocksdb::WriteOptions(), &batch);

	        return static_cast<bool>(this->status.ok());
	    }

	    /**
	     * get value by key
	     * function get
	     * @param string key
	     * @return string value or NULL (if not key exist)
	     */
	    Php::Value get(Php::Parameters &params) {
	        if (params.size() < 1) {
	            throw Php::Exception("Requires 1 parameters: key");
	            return false;
	        }
	        //std::string key, value;
	        std::string value, k1 = params[0].stringValue();
	        //rocksdb::Slice key( (std::string)params[0].stringValue());
	        rocksdb::Slice key( k1);

	        //key   = params[0].stringValue();

	        const Php::Value  phpnull;

	        this->status = db->Get(rocksdb::ReadOptions(), key, &value);
	        //std::cout << "Get(" << key <<") = " << value << std::endl;
	        std::cout << "Get_c(" << key.ToString().c_str()<< ") : " <<" Get_s(" << key.ToString() <<") = " << value << std::endl;
	        std::cout << "Slice.size()=" << key.size() <<" string.size()=" << k1.size() << std::endl;

	        if (!this->status.ok()) {
	            return phpnull;
	        }
	        return value;
	    }

	    /**
	     * get array values by array keys
	     * function mget
	     * @param Php::Array keys
	     * @return Php::Array rezult
	     */
	    Php::Value mget(Php::Parameters &params) {
	        if (params.size() < 1) {
	            throw Php::Exception("Requires 1 parameters: array keys");
	            return false;
	        }
	        if(!params[0].isArray()) {
	            throw Php::Exception("Requires array");
	            return false;
	        }

	        Php::Value rez;
	        const Php::Value  phpnull;
	        unsigned int arrSize = params[0].size();

	        std::vector<std::string> ks;
	        ks.reserve(arrSize);

	        // Slice keys that only refers to 'ks'!
	        //std::vector<rocksdb::Slice> keys(arrSize);
	        std::vector<rocksdb::Slice> keys;
	        keys.reserve(arrSize);

	        std::vector<std::string> values(arrSize);
	        std::vector<rocksdb::Status> statuses(arrSize);

	        std::string k;
	        for (unsigned int i = 0; i < arrSize; i++) {
	            //keys[i] = (const char *) params[0][i];
	            //keys[i] = params[0][i].value().stringValue().c_str();
	            //keys[i] = rocksdb::Slice(params[0][i].value().stringValue());
	            
	            //k = (std::string) (params[0][i].value().stringValue()+'\0');
	            //k = (std::string) (params[0][i].value().stringValue());
	            

	            //k = params[0][i].value().stringValue();
	            //ks[i] = "key#"+ std::to_string(i);
	            ks.push_back( "key#"+ std::to_string(i) );


	            //keys[i] = rocksdb::Slice(k);


	            //keys[i] = rocksdb::Slice( (const char *) k.c_str()+'\0', k.size()+1);

	            //keys[i] = rocksdb::Slice( k.c_str());
	            //keys[i] = (const char *) k.c_str();
	            //keys.push_back( rocksdb::Slice((const char*)(k.c_str()+'\0'), size_t(k.size()+1) ) );
	            
	            //keys.push_back( rocksdb::Slice(k) );

	            //keys.push_back( rocksdb::Slice(k.c_str(), k.size()) );
	            //keys[i] = rocksdb::Slice( k.c_str(), k.size());
	            
	            //keys[i] = rocksdb::Slice( ks[i] );
	            keys.push_back( ks[i] );
	            
	            std::cout << ks[i] <<" <-> "<< keys[i].ToString() << std::endl;
	            
	        }

	        

	        std::cout << "Принт кейс" << std::endl;
	        for (unsigned int i = 0; i < arrSize; i++) {
		        std::cout << "Get_c(" << keys[i].ToString().c_str() << ") : " <<" Get_s(" << keys[i].ToString() <<") " << std::endl;
	        }

	        statuses = db->MultiGet(rocksdb::ReadOptions(), keys, &values);

	        std::cout << "Принт кейс" << std::endl;
	        for (unsigned int i = 0; i < arrSize; i++) {
		        std::cout << "Get_c(" << keys[i].ToString().c_str() << ") : " <<" Get_s(" << keys[i].ToString() <<") " << std::endl;
		        //std::cout << "Slice.size()=" << keys[i].size() <<" string.size()=" << keys[i].ToString().size() << std::endl;
	        }

	        std::cout << "Принт кейс 3" << std::endl;
	        for (unsigned int i = 0; i < arrSize; i++) {
		        std::cout << "Get_c(" << keys[i].ToString().c_str() << ") : " <<" Get_s(" << keys[i].ToString() <<") " << std::endl;
		        
		        std::cout << " Get_a(";
		        for (unsigned int j = 0; j < keys[i].size(); j++) {
		        	std::cout << keys[i][j];
	        	}
	        	std::cout << ") " << std::endl;


	        }



	        std::cout << "Rezult:" << std::endl;
	        for (unsigned int i = 0; i < arrSize; i++) {
	        	k = keys[i].ToString();

	        	std::cout << "Get(" << k <<") = " << values[i] << std::endl;
		        std::cout << "Get_c(" << keys[i].ToString().c_str() << ") : " <<" Get_s(" << keys[i].ToString() <<") = " <<  values[i] << std::endl;
		        std::cout << "Slice.size()=" << keys[i].size() <<" string.size()=" << k.size() << std::endl;


	            rez[std::string(keys[i].ToString())] = (statuses[i].ok()) ? (Php::Value) values[i] : phpnull;
	        }
	        return rez;
	    }

	    /**
	     * fast check exist key
	     * function isset
	     * @param string key
	     * @return bool (true if key exist)
	     */
	    Php::Value isset(Php::Parameters &params) {
	        unsigned int sz = params.size();

	        if (sz < 1) {
	            throw Php::Exception("Requires 1 parameters: key");
	            return false;
	        }
	        std::string key, value;
	        key   = params[0].stringValue();
	        bool r;

	        r = db->KeyMayExist(rocksdb::ReadOptions(), key, &value);
	        //It is not guaranteed that value will be determined
	        //std::cout << "Get(" << key <<") = " << value << std::endl;
	        //std::cout << "sz" << sz << std::endl;

	        /*
	        if (r && sz > 1) {
	            params[1] = (Php::Value) value;
	        }
	        */

	        return r;
	    }

	    /**
	     * remove key from db
	     * function del
	     * @param string key
	     * @return bool status rezult
	     */
	    Php::Value del(Php::Parameters &params) {
	        if (params.size() < 1) {
	            throw Php::Exception("Requires 1 parameters: key");
	            return false;
	        }

	        this->status = db->Delete(rocksdb::WriteOptions(), params[0].stringValue());
	        return static_cast<bool>(this->status.ok());
	    }

	    /**
	     * remove keys from db
	     * function mdel
	     * @param Php::Array keys
	     * @return bool status rezult
	     */
	    Php::Value mdel(Php::Parameters &params) {
	        if (params.size() < 1) {
	            throw Php::Exception("Requires 1 parameters: array keys");
	            return false;
	        }
	        if(!params[0].isArray()) {
	            throw Php::Exception("Requires array");
	            return false;
	        }

	        unsigned int arrSize = params[0].size();
	        rocksdb::WriteBatch batch;
	        std::string key;

	        for (unsigned int i = 0; i < arrSize; i++) {
	            key = (const char *) params[0][i];
	            batch.Delete(key);
	        }
	        this->status = db->Write(rocksdb::WriteOptions(), &batch);

	        return static_cast<bool>(this->status.ok());
	    }

	    /**
	     * function getStatus
	     * @param void
	     * @return string status.ToString()
	     */
	    Php::Value getStatus() {
	        return  this->status.ToString();
	    }

	    /**
	     * function incr
	     * @param string key
	     * @param int incval, default: 1
	     * @return void
	     */
	    void incr(Php::Parameters &params) {
	        if (params.size() < 1) {
	            throw Php::Exception("Requires parameter: key");
	            return;
	        }
	        int64_t iv = 1;
	        if (params.size() > 1) {
	            iv = params[1].numericValue();
	        }

	        this->status = db->Merge(rocksdb::WriteOptions(), params[0].stringValue(), std::to_string(iv));
	    }
	};
}



extern "C"
{
    // export the "get_module" function that will be called by the Zend engine
    PHPCPP_EXPORT void *get_module()
    {
        // create extension
        static Php::Extension extension("RocksDBphp","0.1");

        // add the custom class ot the extension
        extension.add("RocksDB", Php::Class<RocksDBPHP::Driver>({
                Php::Public("__construct", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::__construct), {
                    Php::ByVal("path", Php::stringType),
                    Php::ByVal("cifm", Php::boolType)
                }),

                // GET
                Php::Public("get", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::get), {
                    Php::ByVal("key", Php::stringType)
                }),
                Php::Public("__get", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::get), {
                    Php::ByVal("key", Php::stringType)
                }),
                Php::Public("mget", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::mget), {
                    Php::ByVal("keys", Php::arrayType)
                }),

                // DEL
                Php::Public("del", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::del), {
                    Php::ByVal("key", Php::stringType)
                }),
                Php::Public("__unset", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::del), {
                    Php::ByVal("key", Php::stringType)
                }),
                Php::Public("mdel", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::mdel), {
                    Php::ByVal("keys", Php::arrayType)
                }),

                // SET
                Php::Public("set", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::set), {
                    Php::ByVal("key", Php::stringType),
                    Php::ByVal("val", Php::stringType)
                }),
                Php::Public("__set", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::set), {
                    Php::ByVal("key", Php::stringType),
                    Php::ByVal("val", Php::stringType)
                }),
                Php::Public("mset", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::mset), {
                    Php::ByVal("keys", Php::arrayType),
                    Php::ByVal("vals", Php::arrayType)
                }),
                // OTHER
                Php::Public("getStatus", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::getStatus)),

                Php::Public("incr", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::incr), {
                    Php::ByVal("key", Php::stringType),
                    Php::ByVal("incrVal", Php::numericType)
                }),

                Php::Public("isset", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::isset), {
                    Php::ByVal("key", Php::stringType),
                    //Php::ByRef("val", Php::stringType) not work. required fix at PHP-CPP
                }),
                Php::Public("__isset", Php::Method<RocksDBPHP::Driver>(&RocksDBPHP::Driver::isset), {
                    Php::ByVal("key", Php::stringType)
                }),
                 
            }));


        // return the extension module
        return extension.module();
    }
}
