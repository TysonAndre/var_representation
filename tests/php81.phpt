--TEST--
Test var_representation() function on enum
--SKIPIF--
<?php if (PHP_VERSION_ID < 80100) { echo "skip requires php 8.1 enum\n"; } ?>
--FILE--
<?php
enum Suit {
    case Hearts;
}
function dump($value): void {
    echo var_representation($value), "\n";
    $output = var_representation($value, VAR_REPRESENTATION_SINGLE_LINE);
    echo "oneline: $output\n";
}
dump(Suit::Hearts);
dump([['key' => [Suit::Hearts]]]);
?>
--EXPECT--
\Suit::Hearts
oneline: \Suit::Hearts
[
  [
    'key' => [
      \Suit::Hearts,
    ],
  ],
]
oneline: [['key' => [\Suit::Hearts]]]