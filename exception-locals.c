#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <ext/standard/info.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_types.h>
#include "php_exception-locals.h"

static inline zend_class_entry* get_base_exception_ce()
{
	// TODO: use zend_exception_get_base() in PHP7
	return zend_exception_get_default();
}

typedef zend_object_value (*exception_new_t)(zend_class_entry *class_type TSRMLS_DC);
static zend_object_value exception_locals_exception_new(zend_class_entry *class_type TSRMLS_DC);

static HashTable* original_new_exception_hooks;

static exception_new_t find_original_create_object_by_class_entry(zend_class_entry *ce)
{
	exception_new_t* original_create_object;
	if (zend_hash_find(original_new_exception_hooks, ce->name, ce->name_length + 1, (void**) &original_create_object) == SUCCESS) {
		return *original_create_object;
	} else {
		return NULL;
	}
}

static void hook_create_object(zend_class_entry *ce)
{
	exception_new_t* create_object = (exception_new_t*) ce->create_object;

	int ret = zend_hash_add(
		original_new_exception_hooks,
		ce->name,
		ce->name_length + 1,
		&create_object,
		sizeof(exception_new_t),
		NULL
	);

	if (ret == SUCCESS) {
		ce->create_object = exception_locals_exception_new;
	}
}

static void unhook_create_object(zend_class_entry *ce)
{
	exception_new_t* original_create_object;
	if (zend_hash_find(original_new_exception_hooks, ce->name, ce->name_length + 1, (void**) &original_create_object) == SUCCESS) {
		ce->create_object = *original_create_object;
	}
}

// helper function which does all the heavy lifting
static int merge_in_locals(zval* trace)
{
	zend_execute_data *ex;

	// search for last called user function
	ex = EG(current_execute_data);
	while (ex && !ex->op_array) {
		ex = ex->prev_execute_data;
	}
	if (!ex || !ex->op_array->last_var) {
		return FAILURE;
	}

	// build locals arrays
	ulong frameno = 0;
	while (ex) {
		zval* stack_frame;

		MAKE_STD_ZVAL(stack_frame);
		array_init(stack_frame);

		// skip internal handler
		if (
			!ex->op_array &&
			ex->prev_execute_data &&
			ex->prev_execute_data->opline &&
			ex->prev_execute_data->opline->opcode != ZEND_DO_FCALL &&
			ex->prev_execute_data->opline->opcode != ZEND_DO_FCALL_BY_NAME &&
			ex->prev_execute_data->opline->opcode != ZEND_INCLUDE_OR_EVAL
		) {
			ex = ex->prev_execute_data;
		}

		// build one stack frame
		if (ex->op_array) {
			for (int i = ex->op_array->num_args; i < ex->op_array->last_var; i++) {
				if (*EX_CV_NUM(ex, i)) {
					if (UNEXPECTED(**EX_CV_NUM(ex, i) == &EG(uninitialized_zval))) {
						continue;
					}

					Z_ADDREF_P(**EX_CV_NUM(ex, i));
					zend_hash_quick_add(
						Z_ARRVAL_P(stack_frame),
						ex->op_array->vars[i].name,
						ex->op_array->vars[i].name_len + 1,
						ex->op_array->vars[i].hash_value,
						(void**)*EX_CV_NUM(ex, i),
						sizeof(zval*),
						(void**)EX_CV_NUM(ex, i)
					);
				}
			}
		}

		zval **trace_entry;
		if (zend_hash_index_find(Z_ARRVAL_P(trace), frameno, (void**) &trace_entry) == SUCCESS) {
			add_assoc_zval_ex(*trace_entry, "locals", sizeof("locals"), stack_frame);
		}

		ex = ex->prev_execute_data;
		frameno++;
	}

	return SUCCESS;
}

/* {{{ */
static zend_object_value exception_locals_exception_new(zend_class_entry *class_type TSRMLS_DC)
{
	exception_new_t create_object;
	zend_object_value exc;
	zval zval_exc;
	zval* trace;

	// call original constructor
	create_object = find_original_create_object_by_class_entry(class_type);
	if (create_object == NULL) {
		// TODO: raise error
		return exc;
	}
	exc = create_object(class_type);

	// convert zend_object_value into a zval
	Z_OBJVAL(zval_exc) = exc;
	Z_OBJ_HT(zval_exc) = zend_get_std_object_handlers();

	// read trace property
	trace = zend_read_property(get_base_exception_ce(), &zval_exc, "trace", sizeof("trace")-1, 0 TSRMLS_CC);

	// merge in locals
	if (Z_TYPE_P(trace) == IS_ARRAY) {
		merge_in_locals(trace);
	}

	return exc;
}
/* }}} */

static int hook_create_object_cb(zend_class_entry **pce TSRMLS_DC)
{
	if (pce && instanceof_function(*pce, get_base_exception_ce())) {
		hook_create_object(*pce);
	}

	return ZEND_HASH_APPLY_KEEP;
}

static int unhook_create_object_cb(zend_class_entry **pce TSRMLS_DC)
{
	if (pce && instanceof_function(*pce, get_base_exception_ce())) {
		unhook_create_object(*pce);
	}

	return ZEND_HASH_APPLY_KEEP;
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(exception_locals)
{
	// initialize original new_exception hooks hashtable
	ALLOC_HASHTABLE(original_new_exception_hooks);
	zend_hash_init(original_new_exception_hooks, CG(class_table)->nTableSize, NULL, NULL, 1);

	// hook exception_new functions
	hook_create_object(get_base_exception_ce());
	zend_hash_apply(CG(class_table), (apply_func_t) hook_create_object_cb);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(exception_locals)
{
	// restore exception_new hooks
	unhook_create_object(get_base_exception_ce());
	zend_hash_apply(CG(class_table), (apply_func_t) unhook_create_object_cb);

	// destroy original new_exception hooks hashtable
	zend_hash_destroy(original_new_exception_hooks);
	FREE_HASHTABLE(original_new_exception_hooks);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(exception_locals)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "exception-locals support", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ exception_locals_module_entry
 */
zend_module_entry exception_locals_module_entry = {
	STANDARD_MODULE_HEADER,
	"exception-locals",
	NULL,
	PHP_MINIT(exception_locals),
	PHP_MSHUTDOWN(exception_locals),
	NULL,
	NULL,
	PHP_MINFO(exception_locals),
	PHP_EXCEPTION_LOCALS_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_EXCEPTION_LOCALS
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(exception_locals)
#endif
