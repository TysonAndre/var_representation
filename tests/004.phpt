--TEST--
Test var_representation() function with typed properties, references
--SKIPIF--
<?php if (PHP_VERSION_ID < 70400) { echo "skip require php 7.4+\n"; } ?>
--FILE--
<?php
require_once __DIR__ . '/dump.inc';
class X {
    public object $typed;
    public string $typedString;
    public $untyped;
    public int $isUnset;
}
$o = new X();
$o->typed = new ArrayObject([strtoupper('test')]);
$o->typedString = strtolower('TEST');
$o->untyped = new X();
dump($o);
$a = [&$o->typed, &$o->typedString, &$o->untyped, &$o];
dump($a);
?>
--EXPECTF--
\X::__set_state([
  'typed' => \ArrayObject::__set_state([
    'TEST',
  ]),
  'typedString' => 'test',
  'untyped' => \X::__set_state([
    'untyped' => null,
  ]),
])
oneline: \X::__set_state(['typed' => \ArrayObject::__set_state(['TEST']), 'typedString' => 'test', 'untyped' => \X::__set_state(['untyped' => null])])
[
  \ArrayObject::__set_state([
    'TEST',
  ]),
  'test',
  \X::__set_state([
    'untyped' => null,
  ]),
  \X::__set_state([
    'typed' => \ArrayObject::__set_state([
      'TEST',
    ]),
    'typedString' => 'test',
    'untyped' => \X::__set_state([
      'untyped' => null,
    ]),
  ]),
]
oneline: [\ArrayObject::__set_state(['TEST']), 'test', \X::__set_state(['untyped' => null]), \X::__set_state(['typed' => \ArrayObject::__set_state(['TEST']), 'typedString' => 'test', 'untyped' => \X::__set_state(['untyped' => null])])]
