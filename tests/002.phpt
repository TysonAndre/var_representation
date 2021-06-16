--TEST--
test1() Basic test
--EXTENSIONS--
var_representation
--FILE--
<?php
$ret = test1();

var_dump($ret);
?>
--EXPECT--
The extension var_representation is loaded and working!
NULL
