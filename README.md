# var\_representation() : readable alternative to var\_export()

## Introduction

This extension adds a function
`var_representation(mixed $value, int $flags = 0): string` to convert a
variable to a string in a way that fixes the shortcomings of `var_export()`

[![Build Status](https://github.com/TysonAndre/var_representation/actions/workflows/main.yml/badge.svg?branch=main)](https://github.com/TysonAndre/var_representation/actions/workflows/main.yml?query=branch%3Amain)
[![Build Status (Windows)](https://ci.appveyor.com/api/projects/status/9gq4nk1cwmgn88ye?svg=true)](https://ci.appveyor.com/project/TysonAndre/var-representation)

## Installation

This extension requires php 7.2 or newer.

```sh
pecl install var_representation
```

On windows, see https://wiki.php.net/internals/windows/stepbystepbuild_sdk_2 instead

## Polyfill

`composer.phar require tysonandre/var_representation_polyfill` can be used to install a less efficient polyfill:
https://packagist.org/packages/TysonAndre/var_representation_polyfill

## Usage

```php
// uses short arrays, and omits array keys if array_is_list() would be true
php > echo var_representation(['a','b']);
[
  'a',
  'b',
]
// can dump everything on one line.
php > echo var_representation(['a', 'b', 'c'], VAR_REPRESENTATION_SINGLE_LINE);
['a', 'b', 'c']
php > echo var_representation("uses double quotes: \$\"'\\\n");
"uses double quotes: \$\"'\\\n"
```

## Documentation

Refer to https://wiki.php.net/rfc/readable_var_representation for details such as what characters are escaped.

## Background information

It is inconvenient to work with the representation of *var\_export()* in many
ways, especially since that function was introduced in php 4.2.0 and
predates both namespaces and the short `[]` array syntax. However,
because the output format of `var_export()` is depended upon in php's
own unit tests, tests of PECL modules, and the behavior or unit tests of
various applications written in PHP, changing `var_export` itself may
be impractical.

Proposals to add alternatives better to var_export to php itself
[have been declined](https://wiki.php.net/rfc/readable_var_representation)

## Differences from var_export

- Unconditionally return a string instead of printing to standard output.`
- Use `null` instead of `NULL` - the former is recommended by more
  coding guidelines [such as PSR-2](https://www.php-fig.org/psr/psr-2/).
- Escape control characters including tabs, newlines, etc., unlike
  `var_export()`/`var_dump()`.

- Change the way indentation is done for arrays/objects. Always add 2
  spaces for every level of arrays, never 3 in objects, and put the
  array start on the same line as the key for arrays and objects)
- Render lists as `"['item1']"` rather than `"array(\n 0 => 'item1',\n)"`
- Always render empty lists on a single line instead of two lines.
- Prepend `\` to class names so that generated code snippets can be
  used in namespaces without any issues.
- Support the bit flag `VAR_REPRESENTATION_SINGLE_LINE=1` in a new
  optional parameter `int $flags = 0` accepting a bitmask. If the
  value of $flags includes this flags, `var_representation()` will
  return a single-line representation for arrays/objects.
