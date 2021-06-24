/*
  +----------------------------------------------------------------------+
  | var_representation extension for PHP                                 |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"

#if PHP_VERSION_ID < 70200
#error PHP versions prior to php 7.2 are not supported
#endif

#include <ctype.h>
#include "zend_types.h"
#include "zend_smart_str.h"
#include "ext/standard/php_string.h"
#include "ext/standard/info.h"
#include "php_var_representation.h"
#include "var_representation.h"

#if PHP_VERSION_ID >= 80100
#include "zend_enum.h"
#endif

#ifndef ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE
#define ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, type_hint, allow_null, default_value) \
	ZEND_ARG_TYPE_INFO(pass_by_ref, name, type_hint, allow_null)
#endif
#if PHP_VERSION_ID < 80000
#define IS_MIXED 0
#endif
#include "var_representation_arginfo.h"

/* For compatibility with older PHP versions */
#ifndef ZEND_PARSE_PARAMETERS_NONE
#define ZEND_PARSE_PARAMETERS_NONE() \
	ZEND_PARSE_PARAMETERS_START(0, 0) \
	ZEND_PARSE_PARAMETERS_END()
#endif

#if PHP_VERSION_ID < 70400
#define zend_get_properties_for(struc, purpose) Z_OBJPROP_P((struc))

#define zend_release_properties(ht) do {} while (0)
#endif

// php 7.2
#ifndef ZEND_SET_ALIGNED
#define ZEND_SET_ALIGNED(alignment, decl) decl
#endif

static const int VAR_REPRESENTATION_SINGLE_LINE = 1;

#if PHP_VERSION_ID < 70300

// #define zend_string_efree(s) zend_string_release((s))
#define GC_ADDREF(p) (++GC_REFCOUNT((p)))
#define GC_TRY_PROTECT_RECURSION(myht) do { (myht)->u.v.nApplyCount++; } while(0)
#define GC_TRY_UNPROTECT_RECURSION(myht) do { (myht)->u.v.nApplyCount--; } while(0)
#define GC_IS_RECURSIVE(myht) ((myht)->u.v.nApplyCount > 0)

/* {{{ php_charmask
 * Fills a 256-byte bytemask with input. You can specify a range like 'a..z',
 * it needs to be incrementing.
 * Returns: FAILURE/SUCCESS whether the input was correct (i.e. no range errors)
 * Copied from php_charmask in php-src/ext/standard/string.c for compatibility
 */
static inline int php_charmask(const unsigned char *input, size_t len, char *mask)
{
	const unsigned char *end;
	unsigned char c;
	int result = SUCCESS;

	memset(mask, 0, 256);
	for (end = input+len; input < end; input++) {
		c=*input;
		if ((input+3 < end) && input[1] == '.' && input[2] == '.'
				&& input[3] >= c) {
			memset(mask+c, 1, input[3] - c + 1);
			input+=3;
		} else {
			ZEND_ASSERT(!((input+1 < end) && input[0] == '.' && input[1] == '.'));
			mask[c]=1;
		}
	}
	return result;
}
/* }}} */
// Copied from php-src php_addcslashes_str from php 7.3 for a tiny performance boost over var_representation_addcslashes
static zend_string *var_representation_addcslashes_str(const char *str, size_t len, const char *what, size_t wlength)
{
	char flags[256];
	char *target;
	const char *source, *end;
	char c;
	size_t  newlen;
	zend_string *new_str = zend_string_safe_alloc(4, len, 0, 0);

	php_charmask((unsigned char *)what, wlength, flags);

	for (source = str, end = source + len, target = ZSTR_VAL(new_str); source < end; source++) {
		c = *source;
		if (flags[(unsigned char)c]) {
			if ((unsigned char) c < 32 || (unsigned char) c > 126) {
				*target++ = '\\';
				switch (c) {
					case '\n': *target++ = 'n'; break;
					case '\t': *target++ = 't'; break;
					case '\r': *target++ = 'r'; break;
					case '\v': *target++ = 'v'; break;
					case '\f': *target++ = 'f'; break;
					default: target += sprintf(target, "%03o", (unsigned char) c);
				}
				continue;
			}
			*target++ = '\\';
		}
		*target++ = c;
	}
	*target = 0;
	newlen = target - ZSTR_VAL(new_str);
	if (newlen < len * 4) {
		new_str = zend_string_truncate(new_str, newlen, 0);
	}
	return new_str;
}
#define php_addcslashes_str var_representation_addcslashes_str
#endif

/* Based on zend_array_is_list */
/* Check if an array is a list */
static zend_always_inline zend_bool var_representation_array_is_list(zend_array *array)
{
	zend_long expected_idx = 0;
	zend_long num_idx;
	zend_string* str_idx;
	/* Empty arrays are lists */
	if (zend_hash_num_elements(array) == 0) {
		return 1;
	}

	/* Packed arrays are lists */
	if (HT_IS_PACKED(array) && HT_IS_WITHOUT_HOLES(array)) {
		return 1;
	}

	/* Check if the list could theoretically be repacked */
	ZEND_HASH_FOREACH_KEY(array, num_idx, str_idx) {
		if (str_idx != NULL || num_idx != expected_idx++) {
			return 0;
		}
	} ZEND_HASH_FOREACH_END();

	return 1;
}

// Source: https://github.com/php/php-src/blob/master/ext/standard/var.c
#define buffer_append_spaces(buf, num_spaces) \
	do { \
		char *tmp_spaces; \
		size_t tmp_spaces_len; \
		tmp_spaces_len = spprintf(&tmp_spaces, 0,"%*c", num_spaces, ' '); \
		smart_str_appendl(buf, tmp_spaces, tmp_spaces_len); \
		efree(tmp_spaces); \
	} while(0);

static zend_string* var_representation_string_double_quotes(const char *str, size_t len) { /* {{{ */
	/* NOTE: This needs to be thread safe, which is why I gave up and used a constant */
	/* '\1'(0x01) means to use the hexadecimal representation, others mean to use a backslash followed by that character */
	static const char lookup[256] = {
		1,  1,  1,  1,  1,  1,  1,  1,  1,'t','n',  1,  1,'r',  1,  1,  /* 0x00-0x0f '\r', '\n', '\t' */
		1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  /* 0x10-0x1f                  */
		0,  0,'"',  0,'$',  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  /* 0x20-0x2f '"', '$'         */
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  /* 0x30-0x3f                  */
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  /* 0x40-0x4f                  */
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,'\\', 0,  0,  0,  /* 0x50-0x5f - '\\'           */
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  /* 0x60-0x6f                  */
		0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  /* 0x70-0x7f backspace (\x7f) */
		/* don't escape 0x80-0xff for now in case it's valid in whatever encoding the application is using,
		   the missing values are filled with zeroes by the compiler */
	};
	/* Copied from php_bin2hex */
	ZEND_SET_ALIGNED(16, static const char hexconvtab[]) = "0123456789abcdef";

	/* based on php_addcslashes_str - allocate a buffer to encode the worst case of 4 bytes per byte */
	zend_string *new_str = zend_string_safe_alloc(4, len, 0, 0);
	const char *source, *end;
	char *target;
	for (source = str, end = source + len, target = ZSTR_VAL(new_str); source < end; source++) {
		const char c = *source;
		const char replacement = lookup[(unsigned char) c];
		if (replacement) {
			*target++ = '\\';
			if (replacement == '\1') {
				/* encode \x00-\x1f and \x7f excluding \r, \n, \t */
				target[0] = 'x';
				target[1] = hexconvtab[((unsigned char) c) >> 4];
				target[2] = hexconvtab[((unsigned char) c) & 15];
				target += 3;
			} else {
				/* encode \r, \n, \t, \$, \", \\ */
				*target++ = replacement;
			}
			continue;
		}
		*target++ = c;
	}
	*target = 0;
	size_t newlen = target - ZSTR_VAL(new_str);
	if (newlen < len * 4) {
		new_str = zend_string_truncate(new_str, newlen, 0);
	}
	return new_str;
}
/* }}} */


static void var_representation_string(smart_str *buf, const char *str, size_t len) /* {{{ */
{
	for (size_t i = 0; i < len; i++) {
		const unsigned char c = (unsigned char)str[i];
		/* escape using double quotes if the string contains control characters such as newline/tab and backspaces(\x7f) */
		if (c < 0x20 || c == 0x7f) {
			/* This needs to escape the previously scanned characters because this didn't check for $ and \\ */
			smart_str_appendc(buf, '"');
			zend_string *ztmp = var_representation_string_double_quotes(str, len);
			smart_str_append(buf, ztmp);
			smart_str_appendc(buf, '"');
			zend_string_free(ztmp);
			return;
		}
	}
	/* guaranteed not to have '\0' characters at this point. */
	zend_string *ztmp = php_addcslashes_str(str, len, "'\\", 2);

	smart_str_appendc(buf, '\'');
	smart_str_append(buf, ztmp);
	smart_str_appendc(buf, '\'');

	zend_string_free(ztmp);
}
/* }}} */

static void var_representation_encode_object_element(zval *zv, zend_ulong index, zend_string *key, int level, smart_str *buf, zend_bool is_list) /* {{{ */
{
	zend_bool multiline = level >= 0;
	if (multiline) {
		buffer_append_spaces(buf, level + 1);
	}
	if (is_list) {
		ZEND_ASSERT(!key);
	} else {
		if (key != NULL) {
			const char *class_name, *prop_name;
			size_t prop_name_len;

			zend_unmangle_property_name_ex(key, &class_name, &prop_name, &prop_name_len);
			var_representation_string(buf, prop_name, prop_name_len);
		} else {
			smart_str_append_long(buf, (zend_long) index);
		}
		smart_str_appendl(buf, " => ", 4);
	}

	var_representation_ex(zv, multiline ? level + 2 : -1, buf);
	if (multiline) {
		smart_str_appendc(buf, ',');
		smart_str_appendc(buf, '\n');
	}
}
/* }}} */

static void var_representation_encode_array_element(zval *zv, zend_ulong index, zend_string *key, int level, smart_str *buf, zend_bool is_list) /* {{{ */
{
	zend_bool multiline = level >= 0;
	if (key == NULL) { /* numeric key */
		if (multiline) {
			buffer_append_spaces(buf, level+1);
		}
		if (!is_list)  {
			smart_str_append_long(buf, (zend_long) index);
			smart_str_appendl(buf, " => ", 4);
		}
	} else { /* string key */
		ZEND_ASSERT(!is_list);

		if (multiline) {
			buffer_append_spaces(buf, level+1);
		}

		var_representation_string(buf, ZSTR_VAL(key), ZSTR_LEN(key));
		smart_str_appendl(buf, " => ", 4);
	}
	var_representation_ex(zv, multiline ? level + 2 : -1, buf);

	if (multiline) {
		smart_str_appendc(buf, ',');
		smart_str_appendc(buf, '\n');
	}
}
/* }}} */

VAR_REPRESENTATION_API void var_representation_ex(zval *struc, int level, smart_str *buf) /* {{{ */
{
	HashTable *myht;
	char tmp_str[PHP_DOUBLE_MAX_LENGTH];
	zend_ulong index;
	zend_string *key;
	zval *val;
	zend_bool first, is_list;

again:
	switch (Z_TYPE_P(struc)) {
		case IS_FALSE:
			smart_str_appendl(buf, "false", 5);
			break;
		case IS_TRUE:
			smart_str_appendl(buf, "true", 4);
			break;
		case IS_NULL:
			smart_str_appendl(buf, "null", 4);
			break;
		case IS_LONG:
			/* INT_MIN as a literal will be parsed as a float. Emit something like
			 * -9223372036854775807-1 to avoid this. */
			if (Z_LVAL_P(struc) == ZEND_LONG_MIN) {
				smart_str_append_long(buf, ZEND_LONG_MIN+1);
				smart_str_appends(buf, "-1");
				break;
			}
			smart_str_append_long(buf, Z_LVAL_P(struc));
			break;
		case IS_DOUBLE:
			php_gcvt(Z_DVAL_P(struc), (int)PG(serialize_precision), '.', 'E', tmp_str);
			smart_str_appends(buf, tmp_str);
			/* Without a decimal point, PHP treats a number literal as an int.
			 * This check even works for scientific notation, because the
			 * mantissa always contains a decimal point.
			 * We need to check for finiteness, because INF, -INF and NAN
			 * must not have a decimal point added.
			 */
			if (zend_finite(Z_DVAL_P(struc)) && NULL == strchr(tmp_str, '.')) {
				smart_str_appendl(buf, ".0", 2);
			}
			break;
		case IS_STRING:
			var_representation_string(buf, Z_STRVAL_P(struc), Z_STRLEN_P(struc));
			break;
		case IS_ARRAY: {
			myht = Z_ARRVAL_P(struc);
#if PHP_VERSION_ID >= 70300
			if (!(GC_FLAGS(myht) & GC_IMMUTABLE)) {
				if (GC_IS_RECURSIVE(myht)) {
					smart_str_appendl(buf, "null", 4);
					zend_error(E_WARNING, "var_representation does not handle circular references");
					return;
				}
				GC_ADDREF(myht);
				GC_PROTECT_RECURSION(myht);
			}
#else
			if (ZEND_HASH_APPLY_PROTECTION(myht) && myht->u.v.nApplyCount++ > 0) {
				myht->u.v.nApplyCount--;
				smart_str_appendl(buf, "null", 4);
				zend_error(E_WARNING, "var_representation does not handle circular references");
				return;
			}
#endif
			smart_str_appendc(buf, '[');
			first = 1;
			is_list = var_representation_array_is_list(myht);
#if PHP_VERSION_ID < 80100
			ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val)
#else
			ZEND_HASH_FOREACH_KEY_VAL(myht, index, key, val)
#endif
			{
				ZEND_ASSERT(Z_TYPE_P(val) != IS_INDIRECT);
				if (level < 0) {
					if (first) {
						first = 0;
					} else {
						smart_str_appendl(buf, ", ", 2);
					}
				} else if (first) {
					smart_str_appendc(buf, '\n');
					first = 0;
				}
				var_representation_encode_array_element(val, index, key, level, buf, is_list);
			} ZEND_HASH_FOREACH_END();
#if PHP_VERSION_ID >= 70300
			if (!(GC_FLAGS(myht) & GC_IMMUTABLE)) {
				GC_UNPROTECT_RECURSION(myht);
				GC_DELREF(myht);
			}
#else
			if (ZEND_HASH_APPLY_PROTECTION(myht)) {
				myht->u.v.nApplyCount--;
			}
#endif

			if (level > 1 && !first) {
				buffer_append_spaces(buf, level - 1);
			}
			smart_str_appendc(buf, ']');

			break;
		}
		case IS_OBJECT: {
			zend_class_entry *ce = Z_OBJCE_P(struc);
#if PHP_VERSION_ID >= 80100
			if (ce->ce_flags & ZEND_ACC_ENUM) {
				smart_str_appendc(buf, '\\');
				smart_str_append(buf, ce->name);
				zend_object *zobj = Z_OBJ_P(struc);
				zval *case_name_zval = zend_enum_fetch_case_name(zobj);
				smart_str_appendl(buf, "::", 2);
				smart_str_append(buf, Z_STR_P(case_name_zval));
				return;
			}
#endif

			myht = zend_get_properties_for(struc, ZEND_PROP_PURPOSE_VAR_EXPORT);
			if (myht) {
				if (GC_IS_RECURSIVE(myht))
				{
					smart_str_appendl(buf, "null", 4);
					zend_error(E_WARNING, "var_representation does not handle circular references");
					zend_release_properties(myht);
					return;
				} else {
					GC_TRY_PROTECT_RECURSION(myht);
				}
			}

			/* stdClass has no __set_state method, but can be casted to */
			if (ce == zend_standard_class_def) {
				smart_str_appendl(buf, "(object) [", 10);
			} else {
				smart_str_appendc(buf, '\\');
				smart_str_append(buf, ce->name);
				smart_str_appendl(buf, "::__set_state([", 15);
			}

			first = 1;
			if (myht) {
				is_list = var_representation_array_is_list(myht);
				ZEND_HASH_FOREACH_KEY_VAL_IND(myht, index, key, val) {
					if (level < 0) {
						if (first) {
							first = 0;
						} else {
							smart_str_appendl(buf, ", ", 2);
						}
					} else if (first) {
						smart_str_appendc(buf, '\n');
						first = 0;
					}
					var_representation_encode_object_element(val, index, key, level, buf, is_list);
				} ZEND_HASH_FOREACH_END();
				GC_TRY_UNPROTECT_RECURSION(myht);
				zend_release_properties(myht);
			}
			if (level > 1 && !first) {
				buffer_append_spaces(buf, level - 1);
			}
			if (Z_OBJCE_P(struc) == zend_standard_class_def) {
				smart_str_appendc(buf, ']');
			} else {
				smart_str_appendl(buf, "])", 2);
			}

			break;
		}
		case IS_REFERENCE:
			struc = Z_REFVAL_P(struc);
			goto again;
			break;
		case IS_RESOURCE:
			zend_error(E_WARNING, "var_representation does not handle resources");
			/* fall through */
		default:
			ZEND_ASSERT(Z_TYPE_P(struc) == IS_RESOURCE);
			smart_str_appendl(buf, "null", 4);
			break;
	}
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION */
PHP_RINIT_FUNCTION(var_representation)
{
#if defined(ZTS) && defined(COMPILE_DL_VAR_REPRESENTATION)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION */
PHP_MINFO_FUNCTION(var_representation)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "var_representation support", "enabled");
	php_info_print_table_row(2, "var_representation version", PHP_VAR_REPRESENTATION_VERSION);
	php_info_print_table_end();
}
/* }}} */

/* {{{ var_representation_module_entry */
zend_module_entry var_representation_module_entry = {
	STANDARD_MODULE_HEADER,
	"var_representation",					/* Extension name */
	ext_functions,					/* zend_function_entry */
	PHP_MINIT(var_representation),	/* PHP_MINIT - Module initialization */
	NULL,							/* PHP_MSHUTDOWN - Module shutdown */
	PHP_RINIT(var_representation),			/* PHP_RINIT - Request initialization */
	NULL,							/* PHP_RSHUTDOWN - Request shutdown */
	PHP_MINFO(var_representation),			/* PHP_MINFO - Module info */
	PHP_VAR_REPRESENTATION_VERSION,		/* Version */
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

/* {{{ Returns a short, readable string representation of a variable */
PHP_FUNCTION(var_representation)
{
	zval *var;
	zend_long flags = 0;
	smart_str buf = {0};

	ZEND_PARSE_PARAMETERS_START(1, 2)
		Z_PARAM_ZVAL(var)
		Z_PARAM_OPTIONAL
		Z_PARAM_LONG(flags)
	ZEND_PARSE_PARAMETERS_END();

	var_representation_ex(var, (flags & 1) ? -1 : 1, &buf);
	smart_str_0 (&buf);

	RETURN_NEW_STR(buf.s);
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(var_representation)
{
	REGISTER_LONG_CONSTANT("VAR_REPRESENTATION_SINGLE_LINE", VAR_REPRESENTATION_SINGLE_LINE, CONST_CS|CONST_PERSISTENT);
	return SUCCESS;
}
/* }}} */

#ifdef COMPILE_DL_VAR_REPRESENTATION
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(var_representation)
#endif
