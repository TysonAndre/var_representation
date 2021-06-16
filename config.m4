dnl config.m4 for extension var_representation

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary.

dnl If your extension references something external, use 'with':

dnl PHP_ARG_WITH([var_representation],
dnl   [for var_representation support],
dnl   [AS_HELP_STRING([--with-var_representation],
dnl     [Include var_representation support])])

dnl Otherwise use 'enable':

PHP_ARG_ENABLE([var_representation],
  [whether to enable var_representation support],
  [AS_HELP_STRING([--enable-var_representation],
    [Enable var_representation support])],
  [no])

if test "$PHP_VAR_REPRESENTATION" != "no"; then
  dnl Write more examples of tests here...

  dnl Remove this code block if the library does not support pkg-config.
  dnl PKG_CHECK_MODULES([LIBFOO], [foo])
  dnl PHP_EVAL_INCLINE($LIBFOO_CFLAGS)
  dnl PHP_EVAL_LIBLINE($LIBFOO_LIBS, VAR_REPRESENTATION_SHARED_LIBADD)

  dnl If you need to check for a particular library version using PKG_CHECK_MODULES,
  dnl you can use comparison operators. For example:
  dnl PKG_CHECK_MODULES([LIBFOO], [foo >= 1.2.3])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo < 3.4])
  dnl PKG_CHECK_MODULES([LIBFOO], [foo = 1.2.3])

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-var_representation -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/var_representation.h"  # you most likely want to change this
  dnl if test -r $PHP_VAR_REPRESENTATION/$SEARCH_FOR; then # path given as parameter
  dnl   VAR_REPRESENTATION_DIR=$PHP_VAR_REPRESENTATION
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for var_representation files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       VAR_REPRESENTATION_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$VAR_REPRESENTATION_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the var_representation distribution])
  dnl fi

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-var_representation -> add include path
  dnl PHP_ADD_INCLUDE($VAR_REPRESENTATION_DIR/include)

  dnl Remove this code block if the library supports pkg-config.
  dnl --with-var_representation -> check for lib and symbol presence
  dnl LIBNAME=VAR_REPRESENTATION # you may want to change this
  dnl LIBSYMBOL=VAR_REPRESENTATION # you most likely want to change this

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   AC_DEFINE(HAVE_VAR_REPRESENTATION_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your var_representation library.])
  dnl ], [
  dnl   $LIBFOO_LIBS
  dnl ])

  dnl If you need to check for a particular library function (e.g. a conditional
  dnl or version-dependent feature) and you are not using pkg-config:
  dnl PHP_CHECK_LIBRARY($LIBNAME, $LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $VAR_REPRESENTATION_DIR/$PHP_LIBDIR, VAR_REPRESENTATION_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_VAR_REPRESENTATION_FEATURE, 1, [ ])
  dnl ],[
  dnl   AC_MSG_ERROR([FEATURE not supported by your var_representation library.])
  dnl ],[
  dnl   -L$VAR_REPRESENTATION_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(VAR_REPRESENTATION_SHARED_LIBADD)

  dnl In case of no dependencies
  AC_DEFINE(HAVE_VAR_REPRESENTATION, 1, [ Have var_representation support ])

  PHP_NEW_EXTENSION(var_representation, var_representation.c, $ext_shared)
fi
