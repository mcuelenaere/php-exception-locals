#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <php.h>
#include <ext/standard/info.h>
#include <Zend/zend_exceptions.h>
#include <Zend/zend_types.h>
#include "php_exception-locals.h"

// helper function which does all the heavy lifting
static zval* create_locals_zval()
{
	zval* return_value;
	zend_execute_data *ex;

	ALLOC_ZVAL(return_value);

	// search for last called user function
	ex = EG(current_execute_data);
	while (ex && !ex->op_array) {
		ex = ex->prev_execute_data;
	}
	if (!ex || !ex->op_array->last_var) {
		RETVAL_NULL();
		return return_value;
	}

	// build locals array
	array_init(return_value);

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

		add_next_index_zval(return_value, stack_frame);

		ex = ex->prev_execute_data;
	}

	return return_value;
}

static inline zend_class_entry* get_base_exception_ce()
{
	// TODO: use zend_exception_get_base() in PHP7
	return zend_exception_get_default();
}

static zend_object_value (*original_base_exception_new)(zend_class_entry *class_type TSRMLS_DC);

/* {{{ */
static zend_object_value exception_locals_exception_new(zend_class_entry *class_type TSRMLS_DC)
{
	zend_object_value ex;
	zval zval_ex;

	// call original constructor
	ex = original_base_exception_new(class_type);

	// convert zend_object_value into a zval
	Z_OBJVAL(zval_ex) = ex;
	Z_OBJ_HT(zval_ex) = zend_get_std_object_handlers();

	// fill locals value
	zend_update_property(class_type, &zval_ex, "locals", sizeof("locals")-1, create_locals_zval() TSRMLS_CC);

	return ex;
}
/* }}} */

/* {{{ proto array Exception::getLocals()
   Get the exception local variables */
ZEND_METHOD(exception, getLocals)
{
	zval* locals;

	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}

	locals = zend_read_property(get_base_exception_ce(), getThis(), "locals", sizeof("locals") - 1, 0);

	*return_value = *locals;
	zval_copy_ctor(return_value);
	INIT_PZVAL(return_value);
}
/* }}} */

static const zend_function_entry default_exception_extra_functions[] = {
	ZEND_ME(exception, getLocals, NULL, ZEND_ACC_PUBLIC|ZEND_ACC_FINAL)
	ZEND_FE_END
};

static int register_locals_for_class_entry(zend_class_entry **pce TSRMLS_DC)
{
	if (pce && instanceof_function(*pce, get_base_exception_ce())) {
		zend_declare_property_null(*pce, "locals", sizeof("locals")-1, ZEND_ACC_PRIVATE);
		zend_register_functions(*pce, default_exception_extra_functions, &(*pce)->function_table, MODULE_PERSISTENT);
	}

	return ZEND_HASH_APPLY_KEEP;
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(exception_locals)
{
	// hook base_exception_new
	original_base_exception_new = get_base_exception_ce()->create_object;
	get_base_exception_ce()->create_object = exception_locals_exception_new;

	// declare locals property and getLocals() method in BaseException
	zend_declare_property_null(get_base_exception_ce(), "locals", sizeof("locals")-1, ZEND_ACC_PRIVATE);
	zend_register_functions(get_base_exception_ce(), default_exception_extra_functions, &get_base_exception_ce()->function_table, MODULE_PERSISTENT);

	// declare locals property and getLocals() method to all subclasses of BaseException as well
	zend_hash_apply(CG(class_table), (apply_func_t) register_locals_for_class_entry);

	return SUCCESS;
}
/* }}} */

static int unregister_getlocals_method(zend_class_entry **pce TSRMLS_DC)
{
	if (pce && instanceof_function(*pce, get_base_exception_ce())) {
		zend_unregister_functions(default_exception_extra_functions, -1, &(*pce)->function_table);
	}

	return ZEND_HASH_APPLY_KEEP;
}

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(exception_locals)
{
	// restore base_exception_new hook
	get_base_exception_ce()->create_object = original_base_exception_new;

	// remove getLocals() method from exceptions
	zend_unregister_functions(default_exception_extra_functions, -1, &get_base_exception_ce()->function_table);
	zend_hash_apply(CG(class_table), (apply_func_t) unregister_getlocals_method);

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
