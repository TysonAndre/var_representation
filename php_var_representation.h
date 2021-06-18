/*
  +----------------------------------------------------------------------+
  | var_representation extension for PHP                                 |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_VAR_REPRESENTATION_H
# define PHP_VAR_REPRESENTATION_H

/** Module entry of var_representation. */
extern zend_module_entry var_representation_module_entry;
#define phpext_var_representation_ptr &var_representation_module_entry

PHP_MINIT_FUNCTION(var_representation);

# define PHP_VAR_REPRESENTATION_VERSION "0.1.0dev"

# if defined(ZTS) && defined(COMPILE_DL_VAR_REPRESENTATION)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

PHPAPI void php_var_representation_ex(zval *struc, int level, smart_str *buf);

#endif	/* PHP_VAR_REPRESENTATION_H */
