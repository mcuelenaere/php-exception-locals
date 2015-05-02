dnl $Id$
dnl config.m4 for extension exception-locals

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(exception-locals, for exception-locals support,
dnl Make sure that the comment is aligned:
dnl [  --with-exception-locals             Include exception-locals support])

dnl Otherwise use enable:

dnl PHP_ARG_ENABLE(exception-locals, whether to enable exception-locals support,
dnl Make sure that the comment is aligned:
dnl [  --enable-exception-locals           Enable exception-locals support])

if test "$PHP_EXCEPTION-LOCALS" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-exception-locals -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/exception-locals.h"  # you most likely want to change this
  dnl if test -r $PHP_EXCEPTION-LOCALS/$SEARCH_FOR; then # path given as parameter
  dnl   EXCEPTION-LOCALS_DIR=$PHP_EXCEPTION-LOCALS
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for exception-locals files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       EXCEPTION-LOCALS_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$EXCEPTION-LOCALS_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the exception-locals distribution])
  dnl fi

  dnl # --with-exception-locals -> add include path
  dnl PHP_ADD_INCLUDE($EXCEPTION-LOCALS_DIR/include)

  dnl # --with-exception-locals -> check for lib and symbol presence
  dnl LIBNAME=exception-locals # you may want to change this
  dnl LIBSYMBOL=exception-locals # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $EXCEPTION-LOCALS_DIR/$PHP_LIBDIR, EXCEPTION-LOCALS_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_EXCEPTION-LOCALSLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong exception-locals lib version or lib not found])
  dnl ],[
  dnl   -L$EXCEPTION-LOCALS_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(EXCEPTION-LOCALS_SHARED_LIBADD)

  PHP_NEW_EXTENSION(exception-locals, exception-locals.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
