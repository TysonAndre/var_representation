/* var_representation extension for PHP */

#ifndef PHP_VAR_REPRESENTATION_H
# define PHP_VAR_REPRESENTATION_H

extern zend_module_entry var_representation_module_entry;
# define phpext_var_representation_ptr &var_representation_module_entry

# define PHP_VAR_REPRESENTATION_VERSION "0.1.0"

# if defined(ZTS) && defined(COMPILE_DL_VAR_REPRESENTATION)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_VAR_REPRESENTATION_H */
