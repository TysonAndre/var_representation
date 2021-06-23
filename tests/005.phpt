--TEST--
Test var_representation() function with $GLOBAL
--FILE--
<?php
require_once __DIR__ . '/dump.inc';
call_user_func(function () {
    foreach (array_keys($GLOBALS) as $key) {
        if ($key !== 'GLOBALS') {
            unset($GLOBALS[$key]);
        }
    }
});
$test = strtoupper('TEST');
$o = new stdClass();
call_user_func(function () {
    $j = $GLOBALS;
    unset($j['GLOBALS']);
    dump($j);
});
?>
--EXPECTF--
[
  'test' => 'TEST',
  'o' => (object) [],
]
oneline: ['test' => 'TEST', 'o' => (object) []]