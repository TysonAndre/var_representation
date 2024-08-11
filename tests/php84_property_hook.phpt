--TEST--
Test var_representation() function on virtual property with hooks
--SKIPIF--
<?php if (PHP_VERSION_ID < 80400) { echo "skip requires php 8.4 property hooks\n"; } ?>
--FILE--
<?php
class X {
    private $data = 'x';
    public $virtual {
        get => "{$this->data}Suffix";
    }
    public $writeOnly {
        set => "prefix$value"; // shorthand for { $this->writeOnly = "prefix$value"; }
    }
    public $writeOnlyVirtual {
        set { /* this is virtual because backing property is not referenced from within the hook */ }
    }
}
function dump($value): void {
    echo var_representation($value), "\n";
    $output = var_representation($value, VAR_REPRESENTATION_SINGLE_LINE);
    echo "oneline: $output\n";
    var_export($value);
    echo "\n";
}
$x = new X();
$x->writeOnly = '123';
$x->writeOnlyVirtual = 'other';
dump(new X());
?>
--EXPECT--
\X::__set_state([
  'data' => 'x',
  'virtual' => 'xSuffix',
  'writeOnly' => null,
])
oneline: \X::__set_state(['data' => 'x', 'virtual' => 'xSuffix', 'writeOnly' => null])
\X::__set_state(array(
   'data' => 'x',
   'virtual' => 'xSuffix',
   'writeOnly' => NULL,
))
