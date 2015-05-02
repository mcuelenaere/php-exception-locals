#ifndef PHP_EXCEPTION_LOCALS_H
#define PHP_EXCEPTION_LOCALS_H

extern zend_module_entry exception_locals_module_entry;
#define phpext_exception_locals_ptr &exception_locals_module_entry

#define PHP_EXCEPTION_LOCALS_VERSION "0.1.0" /* Replace with version number for your extension */

#ifdef PHP_WIN32
#	define PHP_EXCEPTION_LOCALS_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_EXCEPTION_LOCALS_API __attribute__ ((visibility("default")))
#else
#	define PHP_EXCEPTION_LOCALS_API
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

#endif	/* PHP_EXCEPTION_LOCALS_H */
