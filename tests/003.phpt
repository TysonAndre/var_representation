--TEST--
var_representation_test2() Basic test
--EXTENSIONS--
var_representation
--FILE--
<?php
var_dump(var_representation_test2());
var_dump(var_representation_test2('PHP'));
?>
--EXPECT--
string(11) "Hello World"
string(9) "Hello PHP"
