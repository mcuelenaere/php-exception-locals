#ifndef EXCEPTION_LOCALS_H
#define EXCEPTION_LOCALS_H

extern zend_module_entry exception_locals_module_entry;
#define phpext_exception_locals_ptr &exception_locals_module_entry

#define EXCEPTION_LOCALS_VERSION "0.1.0"

#ifdef PHP_WIN32
#	define EXCEPTION_LOCALS_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define EXCEPTION_LOCALS_API __attribute__ ((visibility("default")))
#else
#	define EXCEPTION_LOCALS_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#ifdef ZTS
#define EXCEPTION_LOCALS_G(v) ZEND_TSRMG(exception_locals_globals_id, zend_exception_locals_globals *, v)
#ifdef COMPILE_DL_EXCEPTION_LOCALS
ZEND_TSRMLS_CACHE_EXTERN();
#endif
#else
#define EXCEPTION_LOCALS_G(v) (exception_locals_globals.v)
#endif

#endif	/* EXCEPTION_LOCALS_H */
