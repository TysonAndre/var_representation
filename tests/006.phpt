--TEST--
Test var_representation() function string encoding
--FILE--
<?php
$s = '';
for ($i = 0; $i < 128; $i++) {
    $s .= chr($i);
}
$repr = var_representation($s);
echo $repr, "\n";
function assert_valid_range_representation(int $min, int $max) {
    $s = '';
    for ($i = $min; $i <= $max; $i++) {
        $s .= chr($i);
    }
    $repr = var_representation($s);
    printf("%d-%d: same=%s\n", $min, $max, var_export(eval("return $repr;") === $s, true));
}
assert_valid_range_representation(0, 127);
assert_valid_range_representation(0, 255);
assert_valid_range_representation(128, 255);
?>
--EXPECT--
"\x00\x01\x02\x03\x04\x05\x06\x07\x08\t\n\x0b\x0c\r\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f !\"#\$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f"
0-127: same=true
0-255: same=true
128-255: same=true