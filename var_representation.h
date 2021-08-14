/*
  +----------------------------------------------------------------------+
  | var_representation extension for PHP                                 |
  | See COPYING file for further copyright information                   |
  +----------------------------------------------------------------------+
  | Author: Tyson Andre <tandre@php.net>                                 |
  +----------------------------------------------------------------------+
*/

#ifndef VAR_REPRESENTATION_H
# define VAR_REPRESENTATION_H
/* Macros based on igbinary */

/* Forward declarations. */
struct zval;

#ifdef PHP_WIN32
#	if defined(VAR_REPRESENTATION_EXPORTS) || (!defined(COMPILE_DL_VAR_REPRESENTATION))
#		define VAR_REPRESENTATION_API __declspec(dllexport)
#	elif defined(COMPILE_DL_VAR_REPRESENTATION)
#		define VAR_REPRESENTATION_API __declspec(dllimport)
#	else
#		define VAR_REPRESENTATION_API /* nothing special */
#	endif
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define VAR_REPRESENTATION_API __attribute__ ((visibility("default")))
#else
#	define VAR_REPRESENTATION_API /* nothing special */
#endif

VAR_REPRESENTATION_API void var_representation_ex_flags(zval *struc, int level, int flags, smart_str *buf);
VAR_REPRESENTATION_API void var_representation_ex(zval *struc, int level, smart_str *buf);

#endif
