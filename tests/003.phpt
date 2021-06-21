--TEST--
Test var_representation() function with internal classes
--FILE--
<?php
function dump($value): void {
    echo var_representation($value), "\n";
    $output = var_representation($value, VAR_REPRESENTATION_SINGLE_LINE);
    echo "oneline: $output\n";
}
dump(new ArrayObject([]));
// use strtolower because it is locale dependent and creates temporary strings instead of constants
dump(new ArrayObject(['first', strtolower('SECOND')]));
dump(new SplFixedArray(0));
dump(new SplFixedArray(2));
dump(new DateTime('@0', new DateTimeZone('Pacific/Nauru')));
dump(function () {});
dump(new class() {});
$o = new class() { public $var; };
$o->var = strtoupper('test');
dump($o);
?>
--EXPECTF--
\ArrayObject::__set_state([])
oneline: \ArrayObject::__set_state([])
\ArrayObject::__set_state([
  'first',
  'second',
])
oneline: \ArrayObject::__set_state(['first', 'second'])
\SplFixedArray::__set_state([])
oneline: \SplFixedArray::__set_state([])
\SplFixedArray::__set_state([
  null,
  null,
])
oneline: \SplFixedArray::__set_state([null, null])
\DateTime::__set_state([
  'date' => '1970-01-01 00:00:00.000000',
  'timezone_type' => 1,
  'timezone' => '+00:00',
])
oneline: \DateTime::__set_state(['date' => '1970-01-01 00:00:00.000000', 'timezone_type' => 1, 'timezone' => '+00:00'])
\Closure::__set_state([])
oneline: \Closure::__set_state([])
\class@anonymous%s::__set_state([])
oneline: \class@anonymous%s::__set_state([])
\class@anonymous%s::__set_state([
  'var' => 'TEST',
])
oneline: \class@anonymous%s::__set_state(['var' => 'TEST'])
