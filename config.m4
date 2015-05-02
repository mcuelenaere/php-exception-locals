PHP_ARG_ENABLE(exception_locals, whether to enable exception-locals support, [  --enable-exception-locals           Enable exception-locals support])

if test "$PHP_EXCEPTION-LOCALS" != "no"; then
  PHP_NEW_EXTENSION(exception_locals, exception-locals.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi