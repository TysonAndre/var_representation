--TEST--
Test var_representation() function (PHP 7.4+)
--SKIPIF--
<?php if (PHP_VERSION_ID < 70400) { echo "skip requires php 7.4+"; } ?>
--FILE--
<?php
function dump($value): void {
    echo var_representation($value), "\n";
    $output = var_representation($value, VAR_REPRESENTATION_SINGLE_LINE);
    echo "oneline: $output\n";
}
class Example {
    public $untyped1;
    public $untyped2 = null;
    public $untyped3 = 3;
    public int $typed1;
    public array $typed2 = [];
    // rendering does not depend on existence of public function __set_state(), like var_export.
}
$x = new Example();
unset($x->untyped1);  // unset properties/uninitialized typed properties are not shown, like var_export
$x->typed2 = [new Example()];
dump($x);
?>
--EXPECT--
\Example::__set_state([
  'untyped2' => null,
  'untyped3' => 3,
  'typed2' => [
    \Example::__set_state([
      'untyped1' => null,
      'untyped2' => null,
      'untyped3' => 3,
      'typed2' => [],
    ]),
  ],
])
oneline: \Example::__set_state(['untyped2' => null, 'untyped3' => 3, 'typed2' => [\Example::__set_state(['untyped1' => null, 'untyped2' => null, 'untyped3' => 3, 'typed2' => []])]])