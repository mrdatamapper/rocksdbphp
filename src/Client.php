<?php
/**
 *  API for RocksServer
 *
 *  @author valmat <ufabiz@gmail.com>
 *  @github https://github.com/valmat/rocksdbphp
 */
namespace RocksServer\RocksDB;
// namespace RocksServer;

class Exception extends \Exception {}

class Client {
    /**
      *   host and port
      */
    private $_host;
    private $_port;
    
    function __construct($host='localhost', $port = 5577) {
        $this->_host = $host;
        $this->_port = $port;
    }
    
    /**
      *  get value by key
      */
    public function get($key) {
        return $this->httpGet('get', $key)->getValue();
    }
    
    /**
      *  Check if key exist
      *  @return bool
      */
    public function keyExist($key, &$val = NULL) {
        $resp = $this->httpGet('exist', $key);
        $rez = $resp->isOk();
        $val = $rez ? $resp->getValue() : NULL;
        return $rez;
    }
    
    /**
      *  multi get key-value pairs by keys
      *  @return MgetIterator
      */
    public function mget($keys) {
        return $this->httpGet('mget', implode('&', $keys))->getMultiValue();
    }
    
    /**
      *  set value for key
      *  @return bool
      */
    public function set($key, $val) {
        return $this->httpPost('set', "$key\n".strlen($val)."\n$val")->isOk();
    }
    
    /**
      *  multi set values for keys
      *  @return bool
      */
    public function mset($data) {
        return $this->httpPost('mset', $this->data2str($data) )->isOk();
    }
    
    /**
      *  remove key from db
      *  @return bool
      */
    public function del($key) {
        return $this->httpPost('del', $key)->isOk();
    }
    
    /**
      *  Multi remove keys from db
      *  @return bool
      */
    public function mdel($keys) {
        return $this->httpPost('mdel', implode("\n", $keys) )->isOk();
    }
    
    /**
      *  incriment value by key
      *  @return bool
      */
    public function incr($key, $value = NULL) {
        return  (
                    $value ?
                    $this->httpPost('incr', "$key\n$value" )->isOk() :
                    $this->httpPost('incr', $key )->isOk()
                );
    }
    
    /**
      *  multi get all key-value pairs (by key-prefix)
      *  @return MgetIterator
      */
    public function getall($prefix = NULL) {
        return (NULL === $prefix) ?
            $this->httpGet('tail')->getMultiValue() :
            $this->httpGet('prefit', $prefix)->getMultiValue();
    }
    
    /**
      *  backup database
      *  @return Response
      */
    public function backup() {
        return $this->httpPost('backup')->isOk();
    }
        
    /**
      *  backup database
      *  @return Response
      */
    public function backupInfo() {
        return new BackupIterator( $this->httpPost('backup/info') );
    }
        
    /**
      *  retrive server statistic
      *  @return Response
      */
    public function stats() {
        return $this->httpPost('stats');
    }
    
    /**
      *  POST request
      */
    protected function httpPost($path, $data = NULL) {
        $buf  = "POST /$path HTTP/1.1\r\n";
        $buf .= "Host:{$this->_host}\r\n";
        
        if(NULL !== $data) {
            $buf .= "Content-Type:application/x-www-form-urlencoded; charset=UTF-8\r\n";
            $buf .= "Content-Length: " . strlen($data) . "\r\n";
            $buf .= "Connection: Close\r\n\r\n";
            $buf .= $data;
        } else {
            $buf .= "Content-Type: charset=UTF-8\r\n";
            $buf .= "Connection: Close\r\n\r\n";
        }
        return $this->request($buf);
    }
    
    /**
      *  GET request
      */
    protected function httpGet($path, $data = NULL) {
        $buf  = $data ? "GET /$path?$data HTTP/1.1\r\n" : "GET /$path HTTP/1.1\r\n";
        $buf .= "Host:{$this->_host}\r\n";
        $buf .= "Content-Type:charset=UTF-8\r\n";
        $buf .= "Connection: Close\r\n\r\n";
        return $this->request($buf);
    }
    
    /**
      *  @param string request
      */
    private function request(&$req) {
        if( !($sock = @fsockopen($this->_host, $this->_port, $errno, $errstr)) ){
            throw new Exception("Unable to create socket: $errstr ($errno)");
        }
        fwrite($sock, $req);
        
        // Check response status
        if( feof($sock) ){
            throw new Exception("Empty response");
        }
        $status = substr(fgets($sock), 9);
        
        if( feof($sock) || '200' !==substr($status, 0, 3) ){
            throw new Exception("Status error: $status");
        }
        
        // skip headers
        $s = '';
        while (!feof($sock) && "\r\n" !== $s) {
            $s =  fgets($sock);
        }
        
        return new Response($sock);
    }
    
    /**
      *  Encodes data to send in a POST request
      */
    private function data2str(array $data) {
        $ret = '';
        foreach($data as $key => $val) {
            $ret .= "$key\n".strlen($val)."\n$val\n";
        }
        return $ret;
    }
    
}
