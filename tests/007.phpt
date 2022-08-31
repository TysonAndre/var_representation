--TEST--
SplFixedArray: Infinite recursion detection edge cases
--FILE--
<?php
call_user_func(function () {
    $x = new SplFixedArray(4);
    $x[0] = NAN;
    $x[1] = 0.0;
    $x[2] = $x;
    $x[3] = $x;
    echo var_representation($x), "\n";
});
?>
--EXPECTF--
Warning: var_representation does not handle circular references in %s on line 8

Warning: var_representation does not handle circular references in %s on line 8
\SplFixedArray::__set_state([
  NAN,
  0.0,
  null,
  null,
])