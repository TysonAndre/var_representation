--TEST--
Test var_representation() function with VAR_REPRESENTATION_UNESCAPED
--FILE--
<?php
function dump($value): void {
    // VAR_REPRESENTATION_UNESCAPED will always encode strings in single quotes for consistency,
    // even when it contains single quotes.
    $repr = var_representation(var_representation($value, VAR_REPRESENTATION_SINGLE_LINE | VAR_REPRESENTATION_UNESCAPED));
    echo "oneline escaped twice: $repr\n";
}
dump(null);
dump(false);
dump(true);
dump(0);
dump(0.0);
dump(new stdClass());
dump((object)['key' => "\$value\tsecond"]);
dump([]);
dump([1,2,3]);
dump(new ArrayObject(['a', ['b']]));
dump("Content-Length: 42\r\n");
// use octal
dump(["Foo\0\r\n\r\001\x19test quotes and special characters\$b'\"\\" => "\0"]);
// does not escape or check validity of bytes "\80-\ff" (e.g. utf-8 data)
dump("▜");
dump((object)["Foo\0\r\n\r\001" => true]);
echo "STDIN is dumped as null, like var_export\n";
dump(STDIN);
echo "The ascii range is encoded as follows:\n";
echo var_representation(implode('', array_map('chr', range(0, 0x7f)))), "\n";
echo "Recursive objects cause a warning, like var_export\n";
$x = new stdClass();
$x->x = $x;
dump($x);
echo "Recursive arrays cause a warning, like var_export\n";

$a = [];
$a[0] = &$a;
$a[1] = 'other';
dump($a);
$ref = 'ref shown as value like var_export';
dump([(object)['key' => (object)['inner' => [1.0]], 'other' => &$ref]]);
class Example {
    public $untyped1;
    public $untyped2 = null;
    public $untyped3 = 3;
    // rendering does not depend on existence of public function __set_state(), like var_export.
}
$x = new Example();
unset($x->untyped1);  // unset properties/uninitialized typed properties are not shown, like var_export
dump($x);
dump("'");
dump("\\\\\\\\\\\\");
?>
--EXPECTF--
oneline escaped twice: 'null'
oneline escaped twice: 'false'
oneline escaped twice: 'true'
oneline escaped twice: '0'
oneline escaped twice: '0.0'
oneline escaped twice: '(object) []'
oneline escaped twice: "(object) ['key' => '\$value\tsecond']"
oneline escaped twice: '[]'
oneline escaped twice: '[1, 2, 3]'
oneline escaped twice: '\\ArrayObject::__set_state([\'a\', [\'b\']])'
oneline escaped twice: "'Content-Length: 42\r\n'"
oneline escaped twice: "['Foo\x00\r\n\r\x01\x19test quotes and special characters\$b\\'\"\\\\' => '\x00']"
oneline escaped twice: '\'▜\''
oneline escaped twice: "(object) ['Foo\x00\r\n\r\x01' => true]"
STDIN is dumped as null, like var_export

Warning: var_representation does not handle resources in %s on line 5
oneline escaped twice: 'null'
The ascii range is encoded as follows:
"\x00\x01\x02\x03\x04\x05\x06\x07\x08\t\n\x0b\x0c\r\x0e\x0f\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1a\x1b\x1c\x1d\x1e\x1f !\"#\$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~\x7f"
Recursive objects cause a warning, like var_export

Warning: var_representation does not handle circular references in %s on line 5
oneline escaped twice: '(object) [\'x\' => null]'
Recursive arrays cause a warning, like var_export

Warning: var_representation does not handle circular references in %s on line 5
oneline escaped twice: '[null, \'other\']'
oneline escaped twice: '[(object) [\'key\' => (object) [\'inner\' => [1.0]], \'other\' => \'ref shown as value like var_export\']]'
oneline escaped twice: '\\Example::__set_state([\'untyped2\' => null, \'untyped3\' => 3])'
oneline escaped twice: '\'\\\'\''
oneline escaped twice: '\'\\\\\\\\\\\\\\\\\\\\\\\\\''
