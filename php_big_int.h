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
#ifndef PHP_BIG_INT_H
#define PHP_BIG_INT_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if HAVE_BIG_INT
extern zend_module_entry bi_module_entry;
#define phpext_big_int_ptr &bi_module_entry

#define BI_MODULE_NAME        "big_int"
#define BI_BUILD_DATE         __DATE__ " " __TIME__
#define BI_VERSION            "1.0.7"
#define BI_INTERNAL_ERROR     "big_int internal error"
#define BI_RESOURCE_NAME      "big_int"
#define BI_MAX_FUNC_ARGS_CNT  3 /* maximum arguments of big_int function, implemented in PHP */


ZEND_MINIT_FUNCTION(bi);
ZEND_MSHUTDOWN_FUNCTION(bi);
ZEND_MINFO_FUNCTION(bi);

/* declaration of functions to be exported */
ZEND_FUNCTION(bi_from_str);
ZEND_FUNCTION(bi_to_str);
ZEND_FUNCTION(bi_base_convert);
ZEND_FUNCTION(bi_add);
ZEND_FUNCTION(bi_sub);
ZEND_FUNCTION(bi_mul);
ZEND_FUNCTION(bi_div);
ZEND_FUNCTION(bi_mod);
ZEND_FUNCTION(bi_cmp);
ZEND_FUNCTION(bi_cmp_abs);
ZEND_FUNCTION(bi_or);
ZEND_FUNCTION(bi_xor);
ZEND_FUNCTION(bi_and);
ZEND_FUNCTION(bi_andnot);
ZEND_FUNCTION(bi_is_zero);
ZEND_FUNCTION(bi_is_one);
ZEND_FUNCTION(bi_abs);
ZEND_FUNCTION(bi_neg);
ZEND_FUNCTION(bi_inc);
ZEND_FUNCTION(bi_dec);
ZEND_FUNCTION(bi_sqr);
ZEND_FUNCTION(bi_sqrt);
ZEND_FUNCTION(bi_sqrt_rem);
ZEND_FUNCTION(bi_muladd);
ZEND_FUNCTION(bi_bit_len);
ZEND_FUNCTION(bi_bit1_cnt);
ZEND_FUNCTION(bi_addmod);
ZEND_FUNCTION(bi_submod);
ZEND_FUNCTION(bi_mulmod);
ZEND_FUNCTION(bi_divmod);
ZEND_FUNCTION(bi_powmod);
ZEND_FUNCTION(bi_factmod);
ZEND_FUNCTION(bi_absmod);
ZEND_FUNCTION(bi_invmod);
ZEND_FUNCTION(bi_sqrmod);
ZEND_FUNCTION(bi_gcd);
ZEND_FUNCTION(bi_next_prime);
ZEND_FUNCTION(bi_div_extended);
ZEND_FUNCTION(bi_sign);
ZEND_FUNCTION(bi_rand);
ZEND_FUNCTION(bi_lshift);
ZEND_FUNCTION(bi_rshift);
ZEND_FUNCTION(bi_set_bit);
ZEND_FUNCTION(bi_clr_bit);
ZEND_FUNCTION(bi_inv_bit);
ZEND_FUNCTION(bi_test_bit);
ZEND_FUNCTION(bi_scan0_bit);
ZEND_FUNCTION(bi_scan1_bit);
ZEND_FUNCTION(bi_hamming_distance);
ZEND_FUNCTION(bi_subint);
ZEND_FUNCTION(bi_cmpmod);
ZEND_FUNCTION(bi_miller_test);
ZEND_FUNCTION(bi_is_prime);
ZEND_FUNCTION(bi_jacobi);
ZEND_FUNCTION(bi_fact);
ZEND_FUNCTION(bi_pow);
ZEND_FUNCTION(bi_serialize);
ZEND_FUNCTION(bi_unserialize);
ZEND_FUNCTION(bi_gcd_extended);
ZEND_FUNCTION(bi_info);

#else /* if HAVE_BIG_INT */
#define phpext_big_int_ptr NULL
#endif

#endif /* ifndef PHP_BIG_INT_H */
