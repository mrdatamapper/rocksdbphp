#!/usr/bin/php
<?php

require 'src/MgetIterator.php';
require 'src/Response.php';
require 'src/BackupIterator.php';
require 'src/Client.php';




$db = new RocksServer\RocksDB\Client('localhost', 5533);

# Test get

// var_export($db->get('key1'));
// var_export($db->get('noexist'));
// var_export($db->get(''));
// echo PHP_EOL, PHP_EOL, PHP_EOL;
// exit;


# Test mget

// $db->mget(['key1','key2','key3'])->show();
// echo PHP_EOL, PHP_EOL, PHP_EOL;
// $db->mget(['key1','k2','kw3', 'noexist'])->show();
// echo PHP_EOL, PHP_EOL, PHP_EOL;
// $db->mget(['key1'])->show();
// echo PHP_EOL, PHP_EOL, PHP_EOL;
// $db->mget([])->show();
// exit;


# Test set
// var_export($db->set('key1', 'val1'));
// var_export($db->set('key2', 'val2'));var_export($db->set('key3', 'val3'));
// echo PHP_EOL, PHP_EOL, PHP_EOL;

// echo PHP_EOL, PHP_EOL, PHP_EOL;
// $v = 'val'.mt_rand(0,500);
// var_export([$db->get('asd'), $db->set('asd',  $v), $v]);
// echo PHP_EOL, PHP_EOL, PHP_EOL;
// $k = 'rnd' . mt_rand(1,10);
// $v = 'v:' . md5(mt_rand(0,500));
// var_export($db->set($k, $v));
// echo PHP_EOL, PHP_EOL;
// var_export($db->get($k));

// echo PHP_EOL, PHP_EOL, PHP_EOL;


# Test mset

// $key1 = 'mskey' . mt_rand(1,100);
// $key2 = 'mskey' . mt_rand(1,100);
// $key3 = 'mskey' . mt_rand(1,100);

// $db->mget([$key1,$key2, $key3])->show();
// echo PHP_EOL, PHP_EOL, PHP_EOL;
// var_export($db->mset([$key1 => 'v1' . mt_rand(0,500), $key2 => 'v2' . mt_rand(0,500), $key3 => 'v3' . mt_rand(0,500)]));
// echo PHP_EOL, PHP_EOL, PHP_EOL;
// $db->mget([$key1, $key2, $key3])->show();



# Test keyExist

// var_export($db->get('key1'));
// echo PHP_EOL;
// var_export([$db->keyExist('key1', $value), $value]);
// echo PHP_EOL;
// var_export([$db->keyExist('key2', $value), $value]);
// echo PHP_EOL;
// var_export([$db->keyExist('mskey' . mt_rand(1,100), $value), $value]);
// echo PHP_EOL;
// var_export([$db->keyExist('noexist', $value), $value]);
// echo PHP_EOL;
// var_export([$db->keyExist('', $value), $value]);
// echo PHP_EOL;


# Test del
/*
var_export($db->set('test1', 'val1'));
echo PHP_EOL;
var_export($db->get('test1'));
echo PHP_EOL;
var_export($db->del('test1'));
echo PHP_EOL;
var_export($db->get('test1'));
echo PHP_EOL;
*/

# Test mdel

// $db->mget(['mdkey1','mdkey2','mdkey3'])->show();
// echo PHP_EOL, PHP_EOL;

// var_export($db->mset(['mdkey1' => 'v1' . mt_rand(0,500), 'mdkey2' => 'v2' . mt_rand(0,500), 'mdkey3' => 'v3' . mt_rand(0,500)]));
// echo PHP_EOL, PHP_EOL;

// $db->mget(['mdkey1','mdkey2','mdkey3'])->show();
// echo PHP_EOL, PHP_EOL;

// var_export($db->mdel(['mdkey1','mdkey2','mdkey3']));
// echo PHP_EOL, PHP_EOL;

// $db->mget(['mdkey1','mdkey2','mdkey3'])->show();
// echo PHP_EOL, PHP_EOL;


# Test incr
/*
var_export($db->get('incrtest'));
echo PHP_EOL, PHP_EOL;

var_export($db->incr('incrtest'));
echo PHP_EOL, PHP_EOL;

var_export($db->get('incrtest'));
echo PHP_EOL, PHP_EOL;

var_export($db->incr('incrtest', 7));
echo PHP_EOL, PHP_EOL;

var_export($db->get('incrtest'));
echo PHP_EOL, PHP_EOL;

var_export($db->incr('incrtest', -2));
echo PHP_EOL, PHP_EOL;

var_export($db->get('incrtest'));
echo PHP_EOL, PHP_EOL;
*/

# Backup
// var_export($db->backup());
// echo PHP_EOL, PHP_EOL;

# Test getall (prefix iterator)
/*$prefix = 'pref1:';
$key1 = $prefix . mt_rand(1,100);
$key2 = $prefix . mt_rand(1,100);
$key3 = $prefix . mt_rand(1,100);

var_export($db->mset([$key1 => 'v1' . mt_rand(0,500), $key2 => 'v2' . mt_rand(0,500), $key3 => 'v3' . mt_rand(0,500)]));
echo PHP_EOL;
//$prefix .= "~";
$db->getall($prefix)->show();
*/

# Test getall (all key-value pairs)
// $db->getall()->show();


# Backup info

var_export($db->backupInfo()->size());
$db->backupInfo()->show();
echo PHP_EOL, PHP_EOL;

