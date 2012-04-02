/***********************************************************************
    Copyright 2004, 2005 Alexander Valyalkin

    These sources is free software. You can redistribute it and/or
    modify it freely. You can use it with any free or commercial
    software.

    These sources is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY. Without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

    You may contact the author by:
       e-mail:  valyala@gmail.com
*************************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"

#if HAVE_BIG_INT
#include "php_big_int.h"
#include "ext/standard/info.h" /* for phpinfo() functions */
#include "big_int_full.h"
#include <stdlib.h> /* for rand() */
#include <time.h> /* for time() */

typedef struct {
    big_int *num;
    char is_not_ref;
} args_entry;

typedef enum {LEFT, RIGHT} shift_direction;

typedef int (*tri_op_func) (const big_int *, const big_int *, const big_int *, big_int *);
typedef int (*tri_op1_func) (const big_int *, const big_int *, size_t, big_int *);
typedef int (*bin_op_func) (const big_int *, const big_int *, big_int *);
typedef int (*bin_op1_func) (const big_int *, size_t, big_int *);
typedef void (*bin_op2_func) (const big_int *, const big_int *, int *);
typedef int (*un_op_func) (const big_int *, big_int *);
typedef void (*un_op1_func) (const big_int *, unsigned int *);
typedef void (*un_op2_func) (const big_int *, int *);

/* compiled function list so Zend knows what's in this module */
zend_function_entry bi_functions[] =
{
    ZEND_FE(bi_from_str, NULL)
    ZEND_FE(bi_to_str, NULL)
    ZEND_FE(bi_base_convert, NULL)
    ZEND_FE(bi_add, NULL)
    ZEND_FE(bi_sub, NULL)
    ZEND_FE(bi_mul, NULL)
    ZEND_FE(bi_div, NULL)
    ZEND_FE(bi_mod, NULL)
    ZEND_FE(bi_cmp, NULL)
    ZEND_FE(bi_cmp_abs, NULL)
    ZEND_FE(bi_or, NULL)
    ZEND_FE(bi_xor, NULL)
    ZEND_FE(bi_and, NULL)
    ZEND_FE(bi_andnot, NULL)
    ZEND_FE(bi_is_zero, NULL)
    ZEND_FE(bi_is_one, NULL)
    ZEND_FE(bi_abs, NULL)
    ZEND_FE(bi_neg, NULL)
    ZEND_FE(bi_inc, NULL)
    ZEND_FE(bi_dec, NULL)
    ZEND_FE(bi_sqr, NULL)
    ZEND_FE(bi_sqrt, NULL)
    ZEND_FE(bi_sqrt_rem, NULL)
    ZEND_FE(bi_muladd, NULL)
    ZEND_FE(bi_bit_len, NULL)
    ZEND_FE(bi_bit1_cnt, NULL)
    ZEND_FE(bi_addmod, NULL)
    ZEND_FE(bi_submod, NULL)
    ZEND_FE(bi_mulmod, NULL)
    ZEND_FE(bi_divmod, NULL)
    ZEND_FE(bi_powmod, NULL)
    ZEND_FE(bi_factmod, NULL)
    ZEND_FE(bi_absmod, NULL)
    ZEND_FE(bi_invmod, NULL)
    ZEND_FE(bi_sqrmod, NULL)
    ZEND_FE(bi_gcd, NULL)
    ZEND_FE(bi_next_prime, NULL)
    ZEND_FE(bi_div_extended, NULL)
    ZEND_FE(bi_sign, NULL)
    ZEND_FE(bi_rand, NULL)
    ZEND_FE(bi_lshift, NULL)
    ZEND_FE(bi_rshift, NULL)
    ZEND_FE(bi_set_bit, NULL)
    ZEND_FE(bi_clr_bit, NULL)
    ZEND_FE(bi_inv_bit, NULL)
    ZEND_FE(bi_test_bit, NULL)
    ZEND_FE(bi_scan0_bit, NULL)
    ZEND_FE(bi_scan1_bit, NULL)
    ZEND_FE(bi_hamming_distance, NULL)
    ZEND_FE(bi_subint, NULL)
    ZEND_FE(bi_cmpmod, NULL)
    ZEND_FE(bi_miller_test, NULL)
    ZEND_FE(bi_is_prime, NULL)
    ZEND_FE(bi_jacobi, NULL)
    ZEND_FE(bi_fact, NULL)
    ZEND_FE(bi_pow, NULL)
    ZEND_FE(bi_serialize, NULL)
    ZEND_FE(bi_unserialize, NULL)
    ZEND_FE(bi_gcd_extended, NULL)
    ZEND_FE(bi_info, NULL)
    {NULL, NULL, NULL}
};

/* compiled module information */
zend_module_entry bi_module_entry =
{
    STANDARD_MODULE_HEADER,
    BI_MODULE_NAME,
    bi_functions,
    ZEND_MINIT(bi),
    ZEND_MSHUTDOWN(bi),
    NULL,
    NULL,
    ZEND_MINFO(bi),
    BI_VERSION,
    STANDARD_MODULE_PROPERTIES
};

/* implement standard "stub" routine to introduce ourselves to Zend */
#if defined(COMPILE_DL_BIG_INT)
ZEND_GET_MODULE(bi)
#endif

/* resource type for [big_int] */
static int resource_type;

static int zval_to_big_int(const char *func_name, zval **tmp, args_entry *arg, int arg_pos TSRMLS_DC)
{
    int rsrc_type, rsrc_id;
    char errbuf[200];
    big_int_str s;

    if (Z_TYPE_PP(tmp) == IS_RESOURCE) {
        arg->is_not_ref = 0; /* arg will not deleted in free_args() */
        rsrc_id = Z_LVAL_PP(tmp);
        arg->num = (big_int *) zend_list_find(rsrc_id, &rsrc_type);
        if (arg->num == NULL) {
            snprintf(errbuf, 200, BI_INTERNAL_ERROR);
            goto error;
        }
        if (rsrc_type != resource_type) {
            snprintf(errbuf, 200, "%s(): wrong resource type passed for argument number [%d] in function. Expected big_int",
                func_name, arg_pos + 1);
            goto error;
        }
    } else {
        /* try to convert argument to [string] */
        arg->is_not_ref = 1; /* this element will be deleted in free_args() */
        arg->num = big_int_create(1);
        if (arg->num == NULL) {
            snprintf(errbuf, 200, BI_INTERNAL_ERROR);
            goto error;
        }
        if (Z_TYPE_PP(tmp) != IS_STRING) {
            SEPARATE_ZVAL(tmp);
            convert_to_string(*tmp);
        }
        s.str = Z_STRVAL_PP(tmp);
        s.len = Z_STRLEN_PP(tmp);
        switch (big_int_from_str(&s, 10, arg->num)) {
        case 0 : break; /* all ok */
        case 2 :
            snprintf(errbuf, 200, "%s(): argument number [%d] contains illegal chars. It can contain only decimal digits 0-9",
                func_name, arg_pos + 1);
            goto error;
        case 3 :
            snprintf(errbuf, 200, "%s(): argument number [%d] cannot be empty",
                func_name, arg_pos + 1);
            goto error;
        default :
            snprintf(errbuf, 200, "%s(): cannot convert argument number [%d] to big_int resource",
                func_name, arg_pos + 1);
            goto error;
        }
    }

    return SUCCESS;

error:
    zend_error(E_WARNING, errbuf);
    return FAILURE;
}

static int get_func_args(const char *func_name, int args_cnt_min, int args_cnt_max,
    int *args_cnt, args_entry *nums TSRMLS_DC)
{
    zval **args[BI_MAX_FUNC_ARGS_CNT];
    int i = 0;
    char errbuf[200];

    if (func_name == NULL) {
        func_name = "unknown";
    }
    assert(args_cnt_min <= args_cnt_max);
    assert(args_cnt_max <= BI_MAX_FUNC_ARGS_CNT);

    *errbuf = '\0'; /* clear error buffer */
    if (*args_cnt < args_cnt_min || *args_cnt > args_cnt_max) {
        snprintf(errbuf, 200, "%s(): wrong numer of parameters. Function expected from %d to %d parameters",
            func_name, args_cnt_min, args_cnt_max);
        goto error;
    }

    /* read parameters of function */
    if (zend_get_parameters_array_ex(*args_cnt, args) == FAILURE) {
        snprintf(errbuf, 200, "%s(): wrong number of parameters", func_name);
        goto error;
    }
    for (i = 0; i < *args_cnt; i++) {
        if (zval_to_big_int(func_name, args[i], &nums[i], i TSRMLS_CC) == FAILURE) {
            /* error message is already sent by zval_to_big_int() */
            goto error;
        }
    }

    return SUCCESS;

error:
    *args_cnt = i;
    if (*errbuf) {
        zend_error(E_WARNING, errbuf);
    }
    return FAILURE;
}

static void free_args(args_entry *args, int cnt)
{
    int i;

    for (i = 0; i < cnt; i++) {
        if (args[i].is_not_ref) {
            big_int_destroy(args[i].num);
        }
    }
}

/**
    Calculates big_int func_name(big_int a)
*/
static void un_op(const char *func_name, un_op_func func, int err_cnt,
    char *const*err, INTERNAL_FUNCTION_PARAMETERS)
{
    const char *errstr = NULL;
    big_int *answer = NULL;
    int args_cnt;
    args_entry args[1] = {0};
    int err_no;

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args(func_name, 1, 1, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    answer = big_int_create(1);
    if (answer == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    err_no = func(args[0].num, answer);
    if (err_no) {
        errstr = (err_no > err_cnt) ? BI_INTERNAL_ERROR : err[err_no - 1];
        goto error;
    }

    free_args(args, 1);
    /* register [answer] as resource */
    ZEND_REGISTER_RESOURCE(return_value, answer, resource_type);

    /* do not free [answer], because it is already registered as resource */
    return;

error:
    big_int_destroy(answer);
    free_args(args, 1);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    Calculates int func_name(big_int a)
*/
static void un_op1(const char *func_name, un_op1_func func, INTERNAL_FUNCTION_PARAMETERS)
{
    const char *errstr = NULL;
    int args_cnt;
    args_entry args[1] = {0};
    unsigned int ans;

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args(func_name, 1, 1, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    func(args[0].num, &ans);

    free_args(args, args_cnt);
    RETVAL_LONG(ans);
    return;

error:
    free_args(args, args_cnt);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    Calculates bool func_name(big_int a)
    (big_int_is_zero, big_int_is_one)
*/
static void un_op2(const char *func_name, un_op2_func func, INTERNAL_FUNCTION_PARAMETERS)
{
    const char *errstr = NULL;
    int args_cnt;
    args_entry args[1] = {0};
    int ans;

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args(func_name, 1, 1, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    func(args[0].num, &ans);

    free_args(args, args_cnt);
    RETVAL_LONG(ans);
    return;

error:
    free_args(args, args_cnt);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    Calculates big_int func_name(big_int a, big_int b)
*/
static void bin_op(const char *func_name, bin_op_func func, int err_cnt,
    char *const*err, INTERNAL_FUNCTION_PARAMETERS)
{
    const char *errstr = NULL;
    big_int *answer = NULL;
    int args_cnt;
    args_entry args[2] = {0};
    int err_no;

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args(func_name, 2, 2, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    answer = big_int_create(1);
    if (answer == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    err_no = func(args[0].num, args[1].num, answer);
    if (err_no) {
        errstr = (err_no > err_cnt) ? BI_INTERNAL_ERROR : err[err_no - 1];
        goto error;
    }

    free_args(args, 2);
    /* register [answer] as resource */
    ZEND_REGISTER_RESOURCE(return_value, answer, resource_type);

    /* do not free [answer], because it is already registered as resource */
    return;

error:
    big_int_destroy(answer);
    free_args(args, 2);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    Calculates big_int func_name(big_int a, int c);
*/
static void bin_op1(const char *func_name, bin_op1_func func, INTERNAL_FUNCTION_PARAMETERS)
{
    const char *errstr = NULL;
    int n_bit;
    big_int *answer = NULL;
    zval *tmp;
    args_entry arg = {0};

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &tmp, &n_bit) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    answer = big_int_create(1);
    if (answer == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    if (zval_to_big_int(func_name, &tmp, &arg, 0 TSRMLS_CC) == FAILURE) {
        /* error message is already sent by zval_to_big_int() */
        goto error;
    }

    if (n_bit >= 0) {
        if (func(arg.num, (size_t) n_bit, answer)) {
            errstr = BI_INTERNAL_ERROR;
            goto error;
        }
    }

    ZEND_REGISTER_RESOURCE(return_value, answer, resource_type);

    free_args(&arg, 1);
    /* do not free answer, because it is registered as resource */
    return;

error:
    big_int_destroy(answer);
    free_args(&arg, 1);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    Calculates int func_name(big_int a, big_int b);
*/
static void bin_op2(const char *func_name, bin_op2_func func, INTERNAL_FUNCTION_PARAMETERS)
{
    const char *errstr = NULL;
    int args_cnt;
    args_entry args[2] = {0};
    int ans;

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args(func_name, 2, 2, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    func(args[0].num, args[1].num, &ans);

    free_args(args, args_cnt);

    RETVAL_LONG(ans);
    return;

error:
    free_args(args, args_cnt);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    Calculates big_int func_name(big_int a, big_int b, big_int c)
*/
static void tri_op(const char *func_name, tri_op_func func, int err_cnt,
    char *const*err, INTERNAL_FUNCTION_PARAMETERS)
{
    const char *errstr = NULL;
    big_int *answer = NULL;
    int args_cnt;
    args_entry args[3] = {0};
    int err_no;

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args(func_name, 3, 3, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    answer = big_int_create(1);
    if (answer == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    err_no = func(args[0].num, args[1].num, args[2].num, answer);
    if (err_no) {
        errstr = (err_no > err_cnt) ? BI_INTERNAL_ERROR : err[err_no - 1];
        goto error;
    }

    free_args(args, 3);
    /* register [answer] as resource */
    ZEND_REGISTER_RESOURCE(return_value, answer, resource_type);

    /* do not free [answer], because it is already registered as resource */
    return;

error:
    big_int_destroy(answer);
    free_args(args, 3);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    Calculates big_int func_name(big_int a, big_int b [, int c]);
    (big_int_or, big_int_xor, big_int_and, big_int_andnot)
*/
static void tri_op1(const char *func_name, tri_op1_func func, INTERNAL_FUNCTION_PARAMETERS)
{
    const char *errstr = NULL;
    int start_pos = 0;
    big_int *answer = NULL;
    zval *tmp[2];
    args_entry args[2] = {0};

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz|l", &tmp[0], &tmp[1], &start_pos) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    if (start_pos < 0) {
        start_pos = 0;
    }

    answer = big_int_create(1);
    if (answer == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    if (zval_to_big_int(func_name, &tmp[0], &args[0], 0 TSRMLS_CC) == FAILURE) {
        /* error message is already sent by zval_to_big_int() */
        goto error;
    }
    if (zval_to_big_int(func_name, &tmp[1], &args[1], 1 TSRMLS_CC) == FAILURE) {
        /* error message is already sent by zval_to_big_int() */
        goto error;
    }

    if (func(args[0].num, args[1].num, (size_t) start_pos, answer)) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    ZEND_REGISTER_RESOURCE(return_value, answer, resource_type);

    free_args(args, 2);
    /* do not free answer, because it is registered as resource */
    return;

error:
    big_int_destroy(answer);
    free_args(args, 2);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

static void do_shift(const char *func_name, shift_direction dir, INTERNAL_FUNCTION_PARAMETERS)
{
    bin_op1_func func;
    const char *errstr = NULL;
    int n_bit;
    big_int *answer = NULL;
    zval *tmp;
    args_entry arg = {0};

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &tmp, &n_bit) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    answer = big_int_create(1);
    if (answer == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    if (zval_to_big_int(func_name, &tmp, &arg, 0 TSRMLS_CC) == FAILURE) {
        /* error message is already sent by zval_to_big_int() */
        goto error;
    }

    switch (dir) {
        case RIGHT:
            func = big_int_rshift;
            break;

        case LEFT:
            func = big_int_lshift;
            break;

        default:
            errstr = BI_INTERNAL_ERROR;
            goto error;
    }

    if (func(arg.num, n_bit, answer)) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    ZEND_REGISTER_RESOURCE(return_value, answer, resource_type);

    free_args(&arg, 1);
    /* do not free answer, because it is registered as resource */
    return;

error:
    big_int_destroy(answer);
    free_args(&arg, 1);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/*******************************************************************/

static void bi_destruction_handler(zend_rsrc_list_entry *rsrc TSRMLS_DC)
{
    /* free allocated memory */
    big_int_destroy((big_int *) rsrc->ptr);
}

ZEND_MINIT_FUNCTION(bi)
{
    time_t t;

    /* register big_int resource type */
    resource_type = zend_register_list_destructors_ex(bi_destruction_handler, NULL, BI_RESOURCE_NAME, module_number);

    /* seed pseudorandom generator */
    t = time(NULL);
    srand((unsigned) t);

    return SUCCESS;
}

ZEND_MSHUTDOWN_FUNCTION(bi)
{
    return SUCCESS;
}

ZEND_MINFO_FUNCTION(bi)
{
    php_info_print_table_start();
    php_info_print_table_row(2, "big_int support", "enabled");
    php_info_print_table_row(2, "big_int library version", BIG_INT_VERSION);
    php_info_print_table_row(2, "php module version", BI_VERSION);
    php_info_print_table_end();
}

/*******************************************************************/
/**
    resource bi_from_str(string number [, int base])

    Creates big_int number from string [number].
    [base] - is base of number. It can be from 2 to 36 inclusive
    Default value of [base] is 10.
*/
ZEND_FUNCTION(bi_from_str)
{
    char *str = NULL;
    int str_len;
    int base;
    big_int_str s;
    big_int *num = NULL;
    const char *errstr = NULL;

    /* read parameters of function */
    base = 10;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|l",
                              &str, &str_len, &base) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    /* initialize [num] */
    num = big_int_create(1);
    if (num == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    /* convert string to big_int */
    s.str = str;
    s.len = str_len;
    switch (big_int_from_str(&s, base, num)) {
        case 0: break; /* no errors */
        case 1: /* wrong base */
            errstr = "bi_from_str(): wrong [base]. It can be from 2 to 36 inclusive";
            goto error;
        case 2: /* string contains wrong chars for chosen base */
            errstr = "bi_from_str(): string contains wrong chars for chosen base";
            goto error;
        case 3:
            errstr = "bi_from_str(): length of the string must be greater than 0";
            goto error;
        default:
            errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    /* register [num] as resource */
    ZEND_REGISTER_RESOURCE(return_value, num, resource_type);

    /* do not free [num], because it is already registered as resource */
    return;

error:
    /* free allocated memory and print error */
    big_int_destroy(num);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    str bi_to_str(resource number [, int base])

    Converts [num] to string with base [base].
    [base] - is base of number. It can be from 2 to 36 inclusive
    Default value of [base] is 10.
*/
ZEND_FUNCTION(bi_to_str)
{
    zval *tmp = NULL;
    int base;
    big_int_str *s_ptr = NULL;
    const char *errstr = NULL;
    args_entry arg = {0};

    /* read parameters of function */
    base = 10;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|l",
                              &tmp, &base) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    /* initialize [s_ptr] */
    s_ptr = big_int_str_create(1);
    if (s_ptr == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    if (zval_to_big_int("bi_to_str", &tmp, &arg, 0 TSRMLS_CC) == FAILURE) {
        /* error message is already sent by zval_to_big_int() */
        goto error;
    }

    switch (big_int_to_str(arg.num, base, s_ptr)) {
        case 0: break; /* no errors */
        case 1: /* wrong base */
            errstr = "bi_to_str(): wrong [base]. It can be from 2 to 36 inclusive";
            goto error;
        default:
            errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    RETVAL_STRINGL(s_ptr->str, (int) s_ptr->len, 1);
    /* free allocated memory */
    free_args(&arg, 1);
    big_int_str_destroy(s_ptr);
    return;

error:
    /* free allocated memory and print error */
    free_args(&arg, 1);
    big_int_str_destroy(s_ptr);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    string bi_base_convert(string number, int base_from, int base_to)

    Returns a string containing number represented in base [base_to].
    [base_from] - is base of [number].
    [base_from] and [base_to] can be from 2 to 36 inclusive.
*/
ZEND_FUNCTION(bi_base_convert)
{
    char *str = NULL;
    int str_len;
    int base_from, base_to;
    big_int_str s, *s_ptr = NULL;
    const char *errstr = NULL;

    /* read parameters of function */
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sll",
                              &str, &str_len, &base_from, &base_to) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    /* initialize [s_ptr] */
    s_ptr = big_int_str_create(1);
    if (s_ptr == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    /* call big_int_base_convert() */
    s.str = str;
    s.len = str_len;
    switch (big_int_base_convert(&s, s_ptr, base_from, base_to)) {
        case 0: break; /* no errors */
        case 1: /* wrong base_from */
            errstr = "bi_base_convert(): wrong [base_from]. It can be from 2 to 36 inclusive";
            goto error;
        case 2: /* wrong base_to */
            errstr = "bi_base_convert(): wrong [base_to]. It can be from 2 to 36 inclusive";
            goto error;
        case 3: /* string contains wrong chars for chosen base */
            errstr = "bi_base_convert(): string contains wrong chars for [base_from]";
            goto error;
        case 4:
            errstr = "bi_base_convert(): length of the string must be greater than 0";
            goto error;
        default:
            errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    /* do not free [num], because it is already registered as resource */
    RETVAL_STRINGL(s_ptr->str, (int) s_ptr->len, 1);
    /* free allocated memory */
    big_int_str_destroy(s_ptr);
    return;

error:
    /* free allocated memory and print error */
    big_int_str_destroy(s_ptr);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    resource bi_add(resource a, resource b)

    Calculates a + b.
*/
ZEND_FUNCTION(bi_add)
{
    bin_op("bi_add", big_int_add, 0, NULL, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_sub(resource a, resource b)

    Calculates a - b.
*/
ZEND_FUNCTION(bi_sub)
{
    bin_op("bi_sub", big_int_sub, 0, NULL, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_mul(resource a, resource b)

    Calculates a * b.
*/
ZEND_FUNCTION(bi_mul)
{
    bin_op("bi_mul", big_int_mul, 0, NULL, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_div(resource a, resource b)

    Calculates a / b.
*/
ZEND_FUNCTION(bi_div)
{
    char *err[] = {
        "bi_div(): division by zero",
    };

    bin_op("bi_div", big_int_div, 1, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_mod(resource a, resource b)

    Calculates a % b.
*/
ZEND_FUNCTION(bi_mod)
{
    char *err[] = {
        "bi_mod(): division by zero",
    };

    bin_op("bi_mod", big_int_mod, 1, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_or(resource a, resource b [, int start_pos])

    Calculates a or b, starting with start_pos.
*/
ZEND_FUNCTION(bi_or)
{
    tri_op1("bi_or", big_int_or, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_and(resource a, resource b [, int start_pos])

    Calculates a and b, starting with start_pos.
*/
ZEND_FUNCTION(bi_and)
{
    tri_op1("bi_and", big_int_and, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_xor(resource a, resource b [, int start_pos])

    Calculates a xor b, starting with start_pos.
*/
ZEND_FUNCTION(bi_xor)
{
    tri_op1("bi_xor", big_int_xor, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_andnot(resource a, resource b [, int start_pos])

    Calculates a andnot b, starting with start_pos.
*/
ZEND_FUNCTION(bi_andnot)
{
    tri_op1("bi_andnot", big_int_andnot, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_abs(resource a)

    Calculates abs(a).
*/
ZEND_FUNCTION(bi_abs)
{
    un_op("bi_abs", big_int_abs, 0, NULL, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_neg(resource a)

    Calculates neg(a).
*/
ZEND_FUNCTION(bi_neg)
{
    un_op("bi_neg", big_int_neg, 0, NULL, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_inc(resource a)

    Calculates inc(a).
*/
ZEND_FUNCTION(bi_inc)
{
    un_op("bi_inc", big_int_inc, 0, NULL, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_dec(resource a)

    Calculates dec(a).
*/
ZEND_FUNCTION(bi_dec)
{
    un_op("bi_dec", big_int_dec, 0, NULL, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_sqr(resource a)

    Calculates sqr(a).
*/
ZEND_FUNCTION(bi_sqr)
{
    un_op("bi_sqr", big_int_sqr, 0, NULL, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_sqrt(resource a)

    Calculates sqrt(a).
*/
ZEND_FUNCTION(bi_sqrt)
{
    char *err[] = {
        "bi_sqrt(): cannot calulcate square root from negative number",
    };

    un_op("bi_sqrt", big_int_sqrt, 1, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_sqrt_rem(resource a)

    Calculates sqrt_rem(a).
*/
ZEND_FUNCTION(bi_sqrt_rem)
{
    char *err[] = {
        "bi_sqrt_rem(): cannot calulcate square root from negative number",
    };

    un_op("bi_sqrt_rem", big_int_sqrt_rem, 1, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_muladd(resource a, resource b, resource c)

    Calculates c + a * b.
*/
ZEND_FUNCTION(bi_muladd)
{
    tri_op("bi_muladd", big_int_muladd, 0, NULL, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_addmod(resource a, resource b, resource c)

    Calculates a + b (mod c).
*/
ZEND_FUNCTION(bi_addmod)
{
    char *err[] = {
        "bi_addmod(): division by zero",
    };

    tri_op("bi_addmod", big_int_addmod, 1, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_submod(resource a, resource b, resource c)

    Calculates a - b (mod c).
*/
ZEND_FUNCTION(bi_submod)
{
    char *err[] = {
        "bi_submod(): division by zero",
    };

    tri_op("bi_submod", big_int_submod, 1, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_mulmod(resource a, resource b, resource c)

    Calculates a * b (mod c).
*/
ZEND_FUNCTION(bi_mulmod)
{
    char *err[] = {
        "bi_mulmod(): division by zero",
    };

    tri_op("bi_mulmod", big_int_mulmod, 1, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_divmod(resource a, resource b, resource c)

    Calculates a / b (mod c).
*/
ZEND_FUNCTION(bi_divmod)
{
    char *err[] = {
        "bi_divmod(): division by zero",
        "bi_divmod(): cannot find inv(b) (mod c)",
    };

    tri_op("bi_divmod", big_int_divmod, 2, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_powmod(resource a, resource b, resource c)

    Calculates pow(a, b) (mod c).
*/
ZEND_FUNCTION(bi_powmod)
{
    char *err[] = {
        "bi_powmod(): division by zero",
        "bi_powmod(): cannot find inv(pow(a, abs(b)) (mod c)",
    };

    tri_op("bi_powmod", big_int_powmod, 2, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_factmod(resource a, resource b)

    Calculates a! (mod b).
*/
ZEND_FUNCTION(bi_factmod)
{
    char *err[] = {
        "bi_factmod(): division by zero",
        "bi_factmod(): factorial number cannot be negative",
    };

    bin_op("bi_factmod", big_int_factmod, 2, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_absmod(resource a, resource b)

    Calculates a (mod b).
*/
ZEND_FUNCTION(bi_absmod)
{
    char *err[] = {
        "bi_absmod(): division by zero",
    };

    bin_op("bi_absmod", big_int_absmod, 1, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_invmod(resource a, resource b)

    Calculates a^(-1) (mod b).
*/
ZEND_FUNCTION(bi_invmod)
{
    char *err[] = {
        "bi_invmod(): division by zero",
        "bi_invmod(): cannot find inv(a) (mod b)",
    };

    bin_op("bi_invmod", big_int_invmod, 2, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_sqrmod(resource a, resource b)

    Calculates a^2 (mod b).
*/
ZEND_FUNCTION(bi_sqrmod)
{
    char *err[] = {
        "bi_sqrmod(): division by zero",
    };

    bin_op("bi_sqrmod", big_int_sqrmod, 1, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_gcd(resource a, resource b)

    Calculates GCD(a, b).
*/
ZEND_FUNCTION(bi_gcd)
{
    char *err[] = {
        "bi_gcd(): division by zero",
    };

    bin_op("bi_gcd", big_int_gcd, 1, err, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_next_prime(resource a)

    Calculates next prime after a.
*/
ZEND_FUNCTION(bi_next_prime)
{
    un_op("bi_next_prime", big_int_next_prime, 0, NULL, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    int bi_cmp(resource a, resource b)

    Compares a with b.
    Returns 1, if a > b
            -1, if a < b
            0, if a == b
*/
ZEND_FUNCTION(bi_cmp)
{
    bin_op2("bi_cmp", big_int_cmp, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    int bi_cmp_abs(resource a, resource b)

    Compares abs(a) with abs(b).
    Returns 1, if abs(a) > abs(b)
            -1, if abs(a) < abs(b)
            0, if abs(a) == abs(b)
*/
ZEND_FUNCTION(bi_cmp_abs)
{
    bin_op2("bi_cmp_abs", big_int_cmp_abs, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    bool bi_is_zero(resource a)

    Returns true if a == 0, else retruns false
*/
ZEND_FUNCTION(bi_is_zero)
{
    un_op2("bi_is_zero", big_int_is_zero, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    bool bi_is_one(resource a)

    Returns true if a == 1, else retruns false
*/
ZEND_FUNCTION(bi_is_one)
{
    un_op2("bi_is_one", big_int_is_one, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    int bi_bit_len(resource a)

    Returns the length of [a]
*/
ZEND_FUNCTION(bi_bit_len)
{
    un_op1("bi_bit_len", big_int_bit_length, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    int bi_bit1_cnt(resource a)

    Returns the number of 1-bits in [a]
*/
ZEND_FUNCTION(bi_bit1_cnt)
{
    un_op1("bi_bit1_cnt", big_int_bit1_cnt, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_rand(int n_bits[, callback function_name])

    Returns pseudorandom number with [n_bits] bit length.

    If [function_name] is set, then use function with name
    [function_name] to generate random number, else use standard
    C function rand()

    Note: this function uses only lower byte of value, returned by
    user function named [function_name].
*/
ZEND_FUNCTION(bi_rand)
{
    const char *errstr = NULL;
    int n_bits;
    big_int *answer = NULL;
    zval *function_name = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l|z", &n_bits, &function_name) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    if (n_bits < 0) {
        errstr = "bi_rand(): [n_bits] must be greater than 0";
        goto error;
    }

    answer = big_int_create(1);
    if (answer == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    if (function_name != NULL) {
        /* following variables are used only, when [function_name] is defined */
        int n_words, i;
        big_int_word tmp;
        big_int_word *num, *num_end;
        unsigned char tmp_val;
        zval *retval1 = NULL;
        TSRMLS_FETCH(); /* for [function_table] variable */

        if (Z_TYPE_P(function_name) != IS_STRING) {
            errstr = "bi_rand(): parameter [function_name] must be a string type";
            goto error;
        }

        MAKE_STD_ZVAL(retval1);

        n_words = (n_bits / BIG_INT_WORD_BITS_CNT) + 1;
        n_bits %= BIG_INT_WORD_BITS_CNT;
        /* allocate memory for [answer] */
        if (big_int_realloc(answer, n_words)) {
            errstr = BI_INTERNAL_ERROR;
            goto error;
        }
        answer->len = n_words;

        /* generate random bitset with [n_words] words length */
        num = answer->num;
        num_end = num + n_words;
        while (num < num_end) {
            tmp = 0;
            i = (int) BIG_INT_WORD_BYTES_CNT;
            while (i--) {
                if (call_user_function(CG(function_table), NULL, function_name, retval1, 0, NULL TSRMLS_CC) != SUCCESS) {
                    errstr = "bi_rand(): user function call failed";
                    goto error;
                }
                if (Z_TYPE_P(retval1) != IS_LONG) {
                    errstr = "bi_rand(): user function must return integer value";
                    goto error;
                }
                tmp_val = (unsigned char) Z_LVAL_P(retval1);

                tmp <<= 8;
                tmp |= tmp_val;
            }
            *num++ = tmp;
        }

        /* clear higer bits in the higer digit */
        *(--num) &= (1 << n_bits) - 1;

        big_int_clear_zeros(answer);
    } else {
        /* use standard C rand() function to generate random data */
        big_int_rand(rand, n_bits, answer);
    }

    ZEND_REGISTER_RESOURCE(return_value, answer, resource_type);

    /* do not free answer, because it is registered as resource */
    return;

error:
    big_int_destroy(answer);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    array bi_div_extended(a, b)

    Calculates q and r:
        a = b * q + r

    Returns array(q, r)
*/
ZEND_FUNCTION(bi_div_extended)
{
    const char *errstr = NULL;
    big_int *q = NULL, *r = NULL;
    int args_cnt;
    args_entry args[2] = {0};
    int is_zero;
    zval *q_zval = NULL, *r_zval = NULL;

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args("bi_div_extended", 2, 2, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    q = big_int_create(1);
    r = big_int_create(1);
    if (q == NULL || r == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    big_int_is_zero(args[1].num, &is_zero);
    if (is_zero) {
        errstr = "bi_div_extended(): division by zero";
        goto error;
    }

    if (big_int_div_extended(args[0].num, args[1].num, q, r)) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    free_args(args, args_cnt);

    MAKE_STD_ZVAL(q_zval);
    MAKE_STD_ZVAL(r_zval);

    /* register [q] and [r] as resources */
    ZEND_REGISTER_RESOURCE(q_zval, q, resource_type);
    ZEND_REGISTER_RESOURCE(r_zval, r, resource_type);

    /* create array(q, r) */
    array_init(return_value);
    add_next_index_zval(return_value, q_zval);
    add_next_index_zval(return_value, r_zval);

    /* do not free [q] and [r], because they are already registered as resources */
    return;

error:
    big_int_destroy(r);
    big_int_destroy(q);
    free_args(args, args_cnt);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    int bi_sign(resource a)

    Returns -1, if a < 0
            1, if a > 0
            0, if a == 0
*/
ZEND_FUNCTION(bi_sign)
{
    const char *errstr = NULL;
    int args_cnt;
    args_entry args[1] = {0};
    sign_type sign;
    int answer, is_zero;

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args("bi_sign", 1, 1, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    big_int_sign(args[0].num, &sign);
    if (sign == MINUS) {
        answer = -1;
    } else {
        big_int_is_zero(args[0].num, &is_zero);
        answer = is_zero ? 0 : 1;
    }

    free_args(args, args_cnt);
    RETVAL_LONG(answer);
    return;

error:
    free_args(args, args_cnt);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    resource bi_rshift(resource a, int n_bit)

    Shifts number [a] to the right by n_bit bits
*/
ZEND_FUNCTION(bi_rshift)
{
    do_shift("bi_rshift", RIGHT, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_lshift(resource a, int n_bit)

    Shifts number [a] to the left by n_bit bits
*/
ZEND_FUNCTION(bi_lshift)
{
    do_shift("bi_lshift", LEFT, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_set_bit(resource a, int n_bit)

    Sets bit number n_bit to 1
*/
ZEND_FUNCTION(bi_set_bit)
{
    bin_op1("bi_set_bit", big_int_set_bit, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_clr_bit(resource a, int n_bit)

    Clears bit number n_bit to 0
*/
ZEND_FUNCTION(bi_clr_bit)
{
    bin_op1("bi_clr_bit", big_int_clr_bit, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    resource bi_inv_bit(resource a, int n_bit)

    Inverts bit number n_bit
*/
ZEND_FUNCTION(bi_inv_bit)
{
    bin_op1("bi_inv_bit", big_int_inv_bit, INTERNAL_FUNCTION_PARAM_PASSTHRU);
}

/**
    int bi_test_bit(resource a, int n_bit)

    Returns 0, if bit number n_bit is 0
            1, if bit number n_bit is 1
*/
ZEND_FUNCTION(bi_test_bit)
{
    const char *errstr = NULL;
    int n_bit;
    zval *tmp;
    args_entry arg = {0};
    int bit_value = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &tmp, &n_bit) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    if (zval_to_big_int("bi_test_bit", &tmp, &arg, 0 TSRMLS_CC) == FAILURE) {
        /* error message is already sent by zval_to_big_int() */
        goto error;
    }

    if (n_bit >= 0) {
        if (big_int_test_bit(arg.num, (size_t) n_bit, &bit_value)) {
            errstr = BI_INTERNAL_ERROR;
            goto error;
        }
    }

    RETVAL_LONG(bit_value);

    free_args(&arg, 1);
    return;

error:
    free_args(&arg, 1);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    int bi_scan0_bit(resource a, int start_bit)

    Returns position of first 0-bit after start_bit in [a]
*/
ZEND_FUNCTION(bi_scan0_bit)
{
    const char *errstr = NULL;
    int n_bit;
    zval *tmp;
    args_entry arg = {0};
    size_t pos = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &tmp, &n_bit) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    if (zval_to_big_int("bi_scan0_bit", &tmp, &arg, 0 TSRMLS_CC) == FAILURE) {
        /* error message is already sent by zval_to_big_int() */
        goto error;
    }

    if (n_bit >= 0) {
        if (big_int_scan0_bit(arg.num, (size_t) n_bit, &pos)) {
            errstr = BI_INTERNAL_ERROR;
            goto error;
        }
    }

    RETVAL_LONG((int)pos);

    free_args(&arg, 1);
    return;

error:
    free_args(&arg, 1);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    int bi_scan1_bit(resource a, int start_bit)

    Returns position of first 1-bit after start_bit in [a]
*/
ZEND_FUNCTION(bi_scan1_bit)
{
    const char *errstr = NULL;
    int n_bit;
    zval *tmp;
    args_entry arg = {0};
    size_t pos = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &tmp, &n_bit) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    if (zval_to_big_int("bi_scan1_bit", &tmp, &arg, 0 TSRMLS_CC) == FAILURE) {
        /* error message is already sent by zval_to_big_int() */
        goto error;
    }

    if (n_bit >= 0) {
        switch (big_int_scan1_bit(arg.num, (size_t) n_bit, &pos)) {
            case 0: break;
            case 1:
                errstr = "bi_scan1_bit(): can't find 1-bit";
                goto error;
            default:
                errstr = BI_INTERNAL_ERROR;
                goto error;
        }
    }

    RETVAL_LONG((int)pos);

    free_args(&arg, 1);
    return;

error:
    free_args(&arg, 1);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    int bi_hamming_distance(resource a, resource b)

    Returns Hamming distance between two numbers.
*/
ZEND_FUNCTION(bi_hamming_distance)
{
    const char *errstr = NULL;
    int args_cnt;
    args_entry args[2] = {0};
    unsigned int dist;

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args("bi_hamming_distance", 2, 2, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    if (big_int_hamming_distance(args[0].num, args[1].num, &dist)) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    RETVAL_LONG((int)dist);

    free_args(args, args_cnt);

    return;

error:
    free_args(args, args_cnt);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    resource bi_subint(resource a, int start_bit, int len [, int is_invert])

    Returns part of number [a], starting with start_bit and length is len.
    If is_invert != 0, then invert number before returning
*/
ZEND_FUNCTION(bi_subint)
{
    const char *errstr = NULL;
    int start_bit, len, is_invert;
    big_int *answer = NULL;
    zval *tmp;
    args_entry arg = {0};

    is_invert = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zll|l",
        &tmp, &start_bit, &len, &is_invert) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    if (len < 0) {
        start_bit -= len;
        len = -len;
    }
    if (start_bit < 0) {
        start_bit = 0;
    }

    answer = big_int_create(1);
    if (answer == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    if (zval_to_big_int("bi_subint", &tmp, &arg, 0 TSRMLS_CC) == FAILURE) {
        /* error message is already sent by zval_to_big_int() */
        goto error;
    }

    if (big_int_subint(arg.num, start_bit, len, is_invert, answer)) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    ZEND_REGISTER_RESOURCE(return_value, answer, resource_type);

    free_args(&arg, 1);
    /* do not free answer, because it is already registered as resource */
    return;

error:
    big_int_destroy(answer);
    free_args(&arg, 1);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    int bi_cmpmod(resource a, resource b, resource c)

    Returns 1, if a > b (mod c)
            0, if a == b (mod c)
            -1, if a < b (mod c)
*/
ZEND_FUNCTION(bi_cmpmod)
{
    const char *errstr = NULL;
    int cmp_flag;
    int args_cnt;
    args_entry args[3] = {0};

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args("bi_cmpmod", 3, 3, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    switch (big_int_cmpmod(args[0].num, args[1].num, args[2].num, &cmp_flag)) {
        case 0: break;
        case 1:
            errstr = "bi_cmpmod(): division by zero";
            goto error;
        default:
            errstr = BI_INTERNAL_ERROR;
            goto error;
    }

    RETVAL_LONG(cmp_flag);

    free_args(args, args_cnt);
    return;

error:
    free_args(args, args_cnt);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    int bi_miller_test(resource a, resource b)

    Primality test number [a] by base [b] (miller test).

    Returns 0, if [a] is composite
            1, if [a] is pseudoprime by base [b]
*/
ZEND_FUNCTION(bi_miller_test)
{
    const char *errstr = NULL;
    int args_cnt;
    args_entry args[2] = {0};
    int is_prime;

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args("bi_miller_test", 2, 2, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    switch (big_int_miller_test(args[0].num, args[1].num, &is_prime)) {
        case 0: break;
        case 1:
            errstr = "bi_miller_test(): [b] is too small. It must be 1 < b < (a - 1)";
            goto error;
        case 2:
            errstr = "bi_miller_test(): [b] is too high. It must be 1 < b < (a - 1)";
            goto error;
        default:
            errstr = BI_INTERNAL_ERROR;
            goto error;
    }

    RETVAL_LONG(is_prime);

    free_args(args, args_cnt);
    return;

error:
    free_args(args, args_cnt);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    int bi_is_prime(resource a)

    Primality test number [a]

    Returns 0, if [a] is composite
            1, if [a] is pseudoprime
            2, if [a] is 100% prime
*/
ZEND_FUNCTION(bi_is_prime)
{
    const char *errstr = NULL;
    int args_cnt;
    args_entry args[1] = {0};
    int is_prime;

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args("bi_is_prime", 1, 1, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    if (big_int_is_prime(args[0].num, 100, 1, &is_prime)) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    RETVAL_LONG(is_prime);

    free_args(args, args_cnt);
    return;

error:
    free_args(args, args_cnt);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    int bi_jacobi(resource a, resource b)

    Calculates Jacobi symbol according to algorithm on page
    http://primes.utm.edu/glossary/page.php?sort=JacobiSymbol

    jacobi = (a|b), if GCD(a, b) == 1
    jacobi = 0, if GCD(a, b) != 1

    Note: Legendre symbol is equal to Jacoby symbol, if [b] is prime.
    Info about Legendre symbol is avaible by the url:
    http://primes.utm.edu/glossary/page.php/LegendreSymbol.html
*/
ZEND_FUNCTION(bi_jacobi)
{
    const char *errstr = NULL;
    int args_cnt;
    args_entry args[2] = {0};
    int jacobi;

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args("bi_jacobi", 2, 2, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    switch (big_int_jacobi(args[0].num, args[1].num, &jacobi)) {
        case 0: break;
        case 1:
            errstr = "bi_jacobi(): second parameter of function must be odd";
            goto error;
        default:
            errstr = BI_INTERNAL_ERROR;
            goto error;
    }

    RETVAL_LONG(jacobi);

    free_args(args, args_cnt);
    return;

error:
    free_args(args, args_cnt);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    resource bi_fact(int a)

    Calculates a!
*/
ZEND_FUNCTION(bi_fact)
{
    int n;
    const char *errstr = NULL;
    big_int *answer = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &n) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    answer = big_int_create(1);
    if (answer == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    switch (big_int_fact(n, answer)) {
        case 0: break;
        case 1:
            errstr = "bi_fact(): [a] cannot be negative";
            goto error;
        default:
            errstr = BI_INTERNAL_ERROR;
            goto error;
    }

    ZEND_REGISTER_RESOURCE(return_value, answer, resource_type);
    return;

error:
    big_int_destroy(answer);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    resource bi_pow(resource a, int power)

    Returns a^power
*/
ZEND_FUNCTION(bi_pow)
{
    const char *errstr = NULL;
    zval *tmp;
    args_entry arg = {0};
    big_int *answer = NULL;
    size_t power;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zl", &tmp, &power) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    if (zval_to_big_int("bi_pow", &tmp, &arg, 0 TSRMLS_CC) == FAILURE) {
        /* error message is already sent by zval_to_big_int() */
        goto error;
    }

    answer = big_int_create(1);
    if (answer == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    if (big_int_pow(arg.num, (int) power, answer)) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    ZEND_REGISTER_RESOURCE(return_value, answer, resource_type);

    free_args(&arg, 1);
    /* do not free answer, because it is already registered as resource */
    return;

error:
    big_int_destroy(answer);
    free_args(&arg, 1);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    string bi_serialize(resource a [, bool is_sign])

    Returns serialized value of [a]
    If is_sign == true, then save sign of the number into first
    byte of returned string.
*/
ZEND_FUNCTION(bi_serialize)
{
    const char *errstr = NULL;
    args_entry arg = {0};
    big_int_str *s_ptr = NULL;
    int is_sign;
    zval *tmp;

    is_sign = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|b", &tmp, &is_sign) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    if (zval_to_big_int("bi_serialize", &tmp, &arg, 0 TSRMLS_CC) == FAILURE) {
        /* error message is already sent by zval_to_big_int() */
        goto error;
    }

    s_ptr = big_int_str_create(1);
    if (s_ptr == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    if (big_int_serialize(arg.num, is_sign, s_ptr)) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    RETVAL_STRINGL(s_ptr->str, (int) s_ptr->len, 1);

    big_int_str_destroy(s_ptr);
    free_args(&arg, 1);
    return;

error:
    big_int_str_destroy(s_ptr);
    free_args(&arg, 1);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    resource bi_unserialize(string str [, bool is_sign])

    Unserialize number from str.
    If is_sign == true then treat first byte of string as sign.
    In this case it must be either 0x01 (plus) or 0xff (minus)
*/
ZEND_FUNCTION(bi_unserialize)
{
    const char *errstr = NULL;
    big_int *answer = NULL;
    char *str;
    int str_len;
    int is_sign;
    big_int_str s;

    is_sign = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|b",
        &str, &str_len, &is_sign) == FAILURE) {
        /* error message was sent by zend_parse_parameters() */
        goto error;
    }

    answer = big_int_create(1);
    if (answer == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    s.str = str;
    s.len = str_len;
    switch (big_int_unserialize(&s, is_sign, answer)) {
        case 0: break;
        case 1:
            errstr = "bi_unserialize(): bytestream is too short";
            goto error;
        case 2:
            errstr = "bi_unserialize(): wrong sign byte in bytestream. It must be 0x01 (plus) or 0xff (minus)";
            goto error;
        default:
            errstr = BI_INTERNAL_ERROR;
            goto error;
    }

    ZEND_REGISTER_RESOURCE(return_value, answer, resource_type);
    return;

error:
    big_int_destroy(answer);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    array bi_gcd_extended(resource a, resource b)

    Returns array(
        0 => gcd(a, b),
        1 => u,
        2 => v,
    );

    Where abs(a) * u + abs(b) * v = gcd(a, b)
*/
ZEND_FUNCTION(bi_gcd_extended)
{
    const char *errstr = NULL;
    int args_cnt;
    args_entry args[2] = {0};
    big_int *gcd = NULL, *u = NULL, *v = NULL;
    zval *gcd_zval = NULL, *u_zval = NULL, *v_zval = NULL;

    args_cnt = ZEND_NUM_ARGS();
    if (get_func_args("bi_bit_len", 2, 2, &args_cnt, args TSRMLS_CC) == FAILURE) {
        goto error;
    }

    gcd = big_int_create(1);
    u = big_int_create(1);
    v = big_int_create(1);
    if (u == NULL || v == NULL) {
        errstr = BI_INTERNAL_ERROR;
        goto error;
    }

    switch (big_int_gcd_extended(args[0].num, args[1].num, gcd, u, v)) {
        case 0: break;
        case 1:
            errstr = "bi_gcd_extended(): division by zero";
            goto error;
        default:
            errstr = BI_INTERNAL_ERROR;
            goto error;
    }

    MAKE_STD_ZVAL(gcd_zval);
    MAKE_STD_ZVAL(u_zval);
    MAKE_STD_ZVAL(v_zval);

    /* register [gcd], [u] and [v] as resources */
    ZEND_REGISTER_RESOURCE(gcd_zval, gcd, resource_type);
    ZEND_REGISTER_RESOURCE(u_zval, u, resource_type);
    ZEND_REGISTER_RESOURCE(v_zval, v, resource_type);

    /* create array(gcd, u, v) */
    array_init(return_value);
    add_next_index_zval(return_value, gcd_zval);
    add_next_index_zval(return_value, u_zval);
    add_next_index_zval(return_value, v_zval);

    free_args(args, args_cnt);
    /* do not free gcd, u and v, because they are already registersd as resources */
    return;

error:
    big_int_destroy(gcd);
    big_int_destroy(v);
    big_int_destroy(u);
    free_args(args, args_cnt);
    if (errstr != NULL) {
        zend_error(E_WARNING, errstr);
    }
    RETVAL_NULL();
}

/**
    string bi_info()

    Returns array(
        'digit_size' => size of big_int word in bits,
        'ext_version' => version of big_int extension,
        'lib_version' => version of big_int library version,
        'ext_build_date' => date of big_int extension building,
        'lib_build_date' => date of big_int library building,
    );
*/
ZEND_FUNCTION(bi_info)
{
    array_init(return_value);
    add_assoc_long(return_value, "digit_size", BIG_INT_DIGIT_SIZE);
    add_assoc_string(return_value, "ext_version", BI_VERSION, 1);
    add_assoc_string(return_value, "lib_version", big_int_version(), 1);
    add_assoc_string(return_value, "ext_build_date", BI_BUILD_DATE, 1);
    add_assoc_string(return_value, "lib_build_date", big_int_build_date(), 1);
}

#endif /* if HAVE_BIG_INT */
