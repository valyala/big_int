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
/**
    simply test of all big_int_* functions
*/
#include <stdarg.h> /* for va_arg */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "basic_funcs.h" /* string and service funcs is here */
#include "bitset_funcs.h"
#include "number_theory.h" /* modular arithmetic functions is here */

/***********************************************/

int is_exit_on_error = 0;

void debug_print(char *format, ...);

void test_str_funcs(void);
void test_service_funcs(void);
void test_bitset_funcs(void);
void test_basic_funcs(void);
void test_modular_arithmetic(void);
void test_number_theory(void);

void debug_print(char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    if (is_exit_on_error) {
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[])
{
    /* determine, exit on error or not */
    if (argc == 1) {
        printf("Usage:\n");
        printf("%s --raise_error=yes\n", argv[0]);
        printf("%s --raise_error=no\n", argv[0]);
        return 0;
    } else if (argc == 2) {
        if (!strcmp(argv[1], "--raise_error=yes")) {
            is_exit_on_error = 1;
        } else if (!strcmp(argv[1], "--raise_error=no")) {
            is_exit_on_error = 0;
        } else {
            printf("wrong parameter [%s]. Expected [--raise_error=yes] or [--raise_error=no]\n", argv[1]);
            return 0;
        }
    } else {
        printf("wrong parameters count.\n");
        printf("Expected only one parameter [--raise_error=yes] or [--raise_error=no]\n");
        return 0;
    }

    printf("tests of BIG_INT project. Size of word is %zu bits\n\n", BIG_INT_WORD_BITS_CNT);
    test_str_funcs();
    test_service_funcs();
    test_bitset_funcs();
    test_basic_funcs();
    test_modular_arithmetic();
    test_number_theory();
    printf("end of BIG_INT tests\n");
    return 0;
}

/**
    Test of bitset funcs
*/
void test_bitset_funcs(void)
{
    /* big_int_rand() */
    {
        size_t test[] = {
            0, 1, 10, 100, 1000, 10000,
        };
        size_t i, len;
        big_int *a = NULL;

        printf("big_int_rand() test...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("error when creating [a]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_rand(rand, test[i], a)) {
                debug_print("error in big_int_rand(%u), i = %u\n", test[i], i);
            }
            /* check the length of generated number. it must be less than test[i] */
            big_int_bit_length(a, (unsigned int *)&len);
            if (len > test[i]) {
                printf("length of generated number cannot be greater than %zu bits, but it is %zu bits. i = %zu\n",
                    test[i], len, i);
            }
        }
        big_int_destroy(a);
        printf("end of big_int_rand() test\n");
    }
    printf("\n");

    /* big_int_bit_length() & big_int_bit1_cnt()*/
    {
        struct {
            char *num;
            unsigned int bit_len;
            unsigned int bit1_cnt;
        } test[] = {
            {"0", 0, 0},
            {"1", 1, 1},
            {"1011", 4, 3},
            {"10111000", 8, 4},
            {"000100000000000", 12, 1},
            {"11111111111111111111111111111111111111111111111111", 50, 50},
        };
        big_int_str *str = NULL;
        big_int *a = NULL;
        size_t i;
        unsigned int bit_len, bit1_cnt;

        printf("big_int_bit_length() & big_int_bit1_cnt() tests...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("error when creating [a]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. i = %u\n",
                    test[i].num, i);
            }
            if (big_int_from_str(str, 2, a)) {
                debug_print("error when converting string [%s] to number [a] with base 2. i = %u\n",
                    str->str, i);
            }
            big_int_bit_length(a, &bit_len);
            if (bit_len != test[i].bit_len) {
                debug_print("wrong result of big_int_bit_len(%s) = %u. Expected %u. i = %u\n",
                    test[i].num, bit_len, test[i].bit_len, i);
            }
            big_int_bit1_cnt(a, &bit1_cnt);
            if (bit1_cnt != test[i].bit1_cnt) {
                debug_print("wrong result of big_int_bit1_cnt(%s) = %u. Expected %u. i = %u\n",
                    test[i].num, bit1_cnt, test[i].bit1_cnt, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(a);
        printf("end of big_int_big_length() & big_int_bit1_cnt() tests\n");
    }
    printf("\n");

    /* big_int_subint() */
    {
        struct {
            char *num;
            size_t start_bit;
            size_t bit_len;
            int is_invert;
            char *answer;
        } test[] = {
            {"0", 10, 20, 0, "0"},
            {"0", 10, 8, 1, "-11111111"},
            {"0", 100, 15, 1, "-111111111111111"},
            {"1011", 0, 1, 0, "1"},
            {"-1011", 0, 4, 1, "100"},
            {"0", 0, 10, 1, "-1111111111"},
        };
        big_int_str *str = NULL;
        big_int *a = NULL;
        size_t i;

        printf("big_int_subint() test...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("error when creating [a]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. i = %u\n", test[i].num, i);
            }
            if (big_int_from_str(str, 2, a)) {
                debug_print("error when converting string [%s] to number [a] with base 2. i = %u\n",
                    str->str, i);
            }
            if (big_int_subint(a, test[i].start_bit, test[i].bit_len, test[i].is_invert, a)) {
                debug_print("error in big_int_subint(%s, %u, %u, %d). i = %u\n",
                    test[i].num, test[i].start_bit, test[i].bit_len, test[i].is_invert, i);
            }
            if (big_int_to_str(a, 2, str)) {
                debug_print("error when converting number big_int_subint(%s, %u, %u, %d) to string [str]. i =%u\n",
                    test[i].num, test[i].start_bit, test[i].bit_len, test[i].is_invert, i);
            }
            if (strcmp(str->str, test[i].answer)) {
                debug_print("wrong result of big_int_subint(%s, %u, %u, %d) = %s. Expected %s. i = %u\n",
                    test[i].num, test[i].start_bit, test[i].bit_len, test[i].is_invert, str->str, test[i].answer, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(a);
        printf("end of big_int_subint() test\n");
    }
    printf("\n");

    /* big_int_or(), big_int_and(), big_int_andnot(), big_int_xor() & big_int_hamming_distance() */
    {
        struct {
            char *a;
            char *b;
            int start_pos;
            char *or;
            char *and;
            char *andnot;
            char *xor;
            unsigned int distance;
        } test[] = {
            {"0", "0", 100, "0", "0", "0", "0", 0},
            {"0", "1", 10, "10000000000", "0", "0", "10000000000", 1},
            {"10001", "1101", 0, "11101", "1", "10000", "11100", 3},
            {"10001", "1101", 1, "11011", "10001", "1", "1011", 3},
        };
        big_int_str *str = NULL;
        big_int *a = NULL, *b = NULL, *c = NULL;
        unsigned int distance;
        size_t i;

        printf("big_int_or(), big_int_and(), big_int_andnot(), big_int_xor() & big_int_hamming_distance() tests...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        c = big_int_create(1);
        if (a == NULL || b == NULL || c == NULL) {
            debug_print("error when creating [a], [b] or [c]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].a, strlen(test[i].a), str)) {
                debug_print("error when copying string [a] = [%s] to [str]. i = %u\n", test[i].a, i);
            }
            if (big_int_from_str(str, 2, a)) {
                debug_print("error when converting string [a] = [%s] to number [a] with base 2. i = %u\n",
                    str->str, i);
            }
            if (big_int_str_copy_s(test[i].b, strlen(test[i].b), str)) {
                debug_print("error when copying string [b] = [%s] to [str]. i = %u\n", test[i].b, i);
            }
            if (big_int_from_str(str, 2, b)) {
                debug_print("error when converting string [b] = [%s] to number [b] with base 2. i = %u\n",
                    str->str, i);
            }
            /* big_int_or() test */
            if (big_int_or(a, b, test[i].start_pos, c)) {
                debug_print("error in big_int_or(%s, %s, %d). i = %u\n", test[i].a, test[i].b, test[i].start_pos, i);
            }
            if (big_int_to_str(c, 2, str)) {
                debug_print("error when converting number big_int_or(%s, %s, %d) to string [str]. i =%u\n",
                    test[i].a, test[i].b, test[i].start_pos, i);
            }
            if (strcmp(str->str, test[i].or)) {
                debug_print("wrong result of big_int_or(%s, %s, %d) = %s. Expected %s. i = %u\n",
                    test[i].a, test[i].b, test[i].start_pos, str->str, test[i].or, i);
            }
            /* big_int_and() test */
            if (big_int_and(a, b, test[i].start_pos, c)) {
                debug_print("error in big_int_and(%s, %s, %d). i = %u\n", test[i].a, test[i].b, test[i].start_pos, i);
            }
            if (big_int_to_str(c, 2, str)) {
                debug_print("error when converting number big_int_and(%s, %s, %d) to string [str]. i =%u\n",
                    test[i].a, test[i].b, test[i].start_pos, i);
            }
            if (strcmp(str->str, test[i].and)) {
                debug_print("wrong result of big_int_and(%s, %s, %d) = %s. Expected %s. i = %u\n",
                    test[i].a, test[i].b, test[i].start_pos, str->str, test[i].and, i);
            }
            /* big_int_andnot() test */
            if (big_int_andnot(a, b, test[i].start_pos, c)) {
                debug_print("error in big_int_andnot(%s, %s, %d). i = %u\n", test[i].a, test[i].b, test[i].start_pos, i);
            }
            if (big_int_to_str(c, 2, str)) {
                debug_print("error when converting number big_int_andnot(%s, %s, %d) to string [str]. i =%u\n",
                    test[i].a, test[i].b, test[i].start_pos, i);
            }
            if (strcmp(str->str, test[i].andnot)) {
                debug_print("wrong result of big_int_andnot(%s, %s, %d) = %s. Expected %s. i = %u\n",
                    test[i].a, test[i].b, test[i].start_pos, str->str, test[i].andnot, i);
            }
            /* big_int_xor() test */
            if (big_int_xor(a, b, test[i].start_pos, c)) {
                debug_print("error in big_int_xor(%s, %s, %d). i = %u\n", test[i].a, test[i].b, test[i].start_pos, i);
            }
            if (big_int_to_str(c, 2, str)) {
                debug_print("error when converting number big_int_xor(%s, %s, %d) to string [str]. i =%u\n",
                    test[i].a, test[i].b, test[i].start_pos, i);
            }
            if (strcmp(str->str, test[i].xor)) {
                debug_print("wrong result of big_int_xor(%s, %s, %d) = %s. Expected %s. i = %u\n",
                    test[i].a, test[i].b, test[i].start_pos, str->str, test[i].xor, i);
            }
            /* big_int_hamming_distance() test */
            if (big_int_hamming_distance(a, b, &distance)) {
                debug_print("error in big_int_hamming_distance(%s, %s). i = %u\n", test[i].a, test[i].b, i);
            }
            if (distance != test[i].distance) {
                debug_print("wrong result of big_int_hamming_distance(%s, %s) = %u. Expected %u. i = %u\n",
                    test[i].a, test[i].b, distance, test[i].distance, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(c);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_or(), big_int_and(), big_int_andnot(), big_int_xor() & big_int_hamming_distance() tests\n");
    }
    printf("\n");

    /* big_int_lshift() & big_int_rshift() */
    {
        struct {
            char *num;
            int n_bits;
            char *lshift;
            char *rshift;
        } test[] = {
            {"0", 100, "0", "0"},
            {"1", 32, "100000000000000000000000000000000", "0"},
            {"1", -33, "0", "1000000000000000000000000000000000"},
            {"-1", 31, "-10000000000000000000000000000000", "0"},
            {"101", 0, "101", "101"},
            {"111", 8, "11100000000", "0"},
            {"1111", -2, "11", "111100"},
        };
        big_int_str *str = NULL;
        big_int *a = NULL, *b = NULL;
        size_t i;

        printf("big_int_lshift() & big_int_rshift() tests...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        if (a == NULL || b == NULL) {
            debug_print("error when creating [a] or [b]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. i = %u\n", test[i].num, i);
            }
            if (big_int_from_str(str, 2, a)) {
                debug_print("error when converting string [%s] to number [a] with base 2. i = %u\n",
                    str->str, i);
            }
            /* big_int_lshift() */
            if (big_int_lshift(a, test[i].n_bits, b)) {
                debug_print("error in big_int_lshift(%s, %u). i = %u\n", test[i].num, test[i].n_bits, i);
            }
            if (big_int_to_str(b, 2, str)) {
                debug_print("error when converting number big_int_lshift(%s, %u) to string [str]. i =%u\n",
                    test[i].num, test[i].n_bits, i);
            }
            if (strcmp(str->str, test[i].lshift)) {
                debug_print("wrong result of big_int_lshift(%s, %u) = %s. Expected %s. i = %u\n",
                    test[i].num, test[i].n_bits, str->str, test[i].lshift, i);
            }
            /* big_int_rshift() */
            if (big_int_rshift(a, test[i].n_bits, b)) {
                debug_print("error in big_int_rshift(%s, %u). i = %u\n", str->str, test[i].n_bits, i);
            }
            if (big_int_to_str(b, 2, str)) {
                debug_print("error when converting number big_int_rshift(%s, %u) to string [str]. i =%u\n",
                    test[i].num, test[i].n_bits, i);
            }
            if (strcmp(str->str, test[i].rshift)) {
                debug_print("wrong result of big_int_rshift(%s, %u) = %s. Expected %s. i = %u\n",
                    test[i].num, test[i].n_bits, str->str, test[i].rshift, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_lshift() & big_int_rshift() tests\n");
    }
    printf("\n");

    /* big_int_set_bit() */
    {
        struct {
            char *num;
            size_t n_bit;
            char *answer;
        } test[] = {
            {"0", 0, "1"},
            {"0", 32, "100000000000000000000000000000000"},
            {"1", 0, "1"},
            {"1", 2, "101"},
        };
        size_t i;
        big_int *a = NULL;
        big_int_str *str = NULL;

        printf("big_int_set_bit() test...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("error when creating [a]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. i =%u\n", test[i].num, i);
            }
            if (big_int_from_str(str, 2, a)) {
                debug_print("error when converting string [%s] to number [a]. i =%u\n", str->str, i);
            }
            if (big_int_set_bit(a, test[i].n_bit, a)) {
                debug_print("error in big_int_set_bit(%s, %u). i = %u\n", test[i].num, test[i].n_bit, i);
            }
            if (big_int_to_str(a, 2, str)) {
                debug_print("error when converting number big_int_set_bit(%s, %u) to string. i = %u\n",
                    test[i].num, test[i].n_bit, i);
            }
            if (strcmp(str->str, test[i].answer)) {
                debug_print("wrong result of big_int_set_bit(%s, %u) = %s. Expected %s. i = %u\n",
                    test[i].num, test[i].n_bit, str->str, test[i].answer, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(a);
        printf("end of big_int_set_bit() test\n");
    }
    printf("\n");

    /* big_int_clr_bit() */
    {
        struct {
            char *num;
            size_t n_bit;
            char *answer;
        } test[] = {
            {"0", 0, "0"},
            {"0", 101, "0"},
            {"1", 0, "0"},
            {"100000000000000000000000000000000", 32, "0"},
            {"100000000000000000000000000000100", 32, "100"},
            {"100001", 5, "1"},
        };
        size_t i;
        big_int *a = NULL;
        big_int_str *str = NULL;

        printf("big_int_clr_bit() test...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("error when creating [a]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. i =%u\n", test[i].num, i);
            }
            if (big_int_from_str(str, 2, a)) {
                debug_print("error when converting string [%s] to number [a]. i =%u\n", str->str, i);
            }
            if (big_int_clr_bit(a, test[i].n_bit, a)) {
                debug_print("error in big_int_clr_bit(%s, %u). i = %u\n", test[i].num, test[i].n_bit, i);
            }
            if (big_int_to_str(a, 2, str)) {
                debug_print("error when converting number big_int_clr_bit(%s, %u) to string. i = %u\n",
                    test[i].num, test[i].n_bit, i);
            }
            if (strcmp(str->str, test[i].answer)) {
                debug_print("wrong result of big_int_clr_bit(%s, %u) = %s. Expected %s. i = %u\n",
                    test[i].num, test[i].n_bit, str->str, test[i].answer, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(a);
        printf("end of big_int_clr_bit() test\n");
    }
    printf("\n");

    /* big_int_inv_bit() */
    {
        struct {
            char *num;
            size_t n_bit;
            char *answer;
        } test[] = {
            {"0", 0, "1"},
            {"1000", 16, "10000000000001000"},
            {"1", 0, "0"},
            {"100000000000000000000000000000000", 32, "0"},
            {"100000000000000000000000000000100", 32, "100"},
            {"100001", 5, "1"},
        };
        size_t i;
        big_int *a = NULL;
        big_int_str *str = NULL;

        printf("big_int_inv_bit() test...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("error when creating [a]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. i =%u\n", test[i].num, i);
            }
            if (big_int_from_str(str, 2, a)) {
                debug_print("error when converting string [%s] to number [a]. i =%u\n", str->str, i);
            }
            if (big_int_inv_bit(a, test[i].n_bit, a)) {
                debug_print("error in big_int_inv_bit(%s, %u). i = %u\n", test[i].num, test[i].n_bit, i);
            }
            if (big_int_to_str(a, 2, str)) {
                debug_print("error when converting number big_int_inv_bit(%s, %u) to string. i = %u\n",
                    test[i].num, test[i].n_bit, i);
            }
            if (strcmp(str->str, test[i].answer)) {
                debug_print("wrong result of big_int_inv_bit(%s, %u) = %s. Expected %s. i = %u\n",
                    test[i].num, test[i].n_bit, str->str, test[i].answer, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(a);
        printf("end of big_int_inv_bit() test\n");
    }
    printf("\n");

    /* big_int_test_bit() */
    {
        struct {
            char *num;
            size_t n_bit;
            int bit_value;
        } test[] = {
            {"0", 0, 0},
            {"1000", 1000, 0},
            {"1", 0, 1},
            {"100000000000000000000000000000000", 32, 1},
            {"100000000000000000000000000000100", 31, 0},
            {"101001", 3, 1},
        };
        size_t i;
        int bit_value;
        big_int *a = NULL;
        big_int_str *str = NULL;

        printf("big_int_test_bit() test...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("error when creating [a]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. i =%u\n", test[i].num, i);
            }
            if (big_int_from_str(str, 2, a)) {
                debug_print("error when converting string [%s] to number [a]. i =%u\n", str->str, i);
            }
            if (big_int_test_bit(a, test[i].n_bit, &bit_value)) {
                debug_print("error in big_int_test_bit(%s, %u). i = %u\n", test[i].num, test[i].n_bit, i);
            }
            if (bit_value != test[i].bit_value) {
                debug_print("wrong result of big_int_test_bit(%s, %u) = %d. Expected %d. i = %u\n",
                    test[i].num, test[i].n_bit, bit_value, test[i].bit_value, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(a);
        printf("end of big_int_test_bit() test\n");
    }
    printf("\n");

    /* big_int_scan1_bit() & big_int_scan0_bit() */
    {
        struct {
            char *num;
            size_t pos_start;
            size_t bit0_pos;
            size_t bit1_pos;
        } test[] = {
            {"1", 0, 1, 0},
            {"100", 0, 0, 2},
            {"101110110110101", 5, 6, 5},
            {"1000000000000000", 15, 16, 15},
        };
        size_t i;
        size_t bit_pos;
        big_int *a = NULL;
        big_int_str *str = NULL;

        printf("big_int_scan1_bit() & big_int_scan0_bit() tests...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("error when creating [a]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. i =%u\n", test[i].num, i);
            }
            if (big_int_from_str(str, 2, a)) {
                debug_print("error when converting string [%s] to number [a]. i =%u\n", str->str, i);
            }
            /* big_int_scan0_bit() */
            if (big_int_scan0_bit(a, test[i].pos_start, &bit_pos)) {
                debug_print("error in big_int_scan0_bit(%s, %u). i = %u\n", test[i].num, test[i].pos_start, i);
            }
            if (bit_pos != test[i].bit0_pos) {
                debug_print("wrong result of big_int_scan0_bit(%s, %u) = %u. Expected %u. i = %u\n",
                    test[i].num, test[i].pos_start, bit_pos, test[i].bit0_pos, i);
            }
            /* big_int_scan1_bit() */
            if (big_int_scan1_bit(a, test[i].pos_start, &bit_pos)) {
                debug_print("error in big_int_scan1_bit(%s, %u). i = %u\n", test[i].num, test[i].pos_start, i);
            }
            if (bit_pos != test[i].bit1_pos) {
                debug_print("wrong result of big_int_scan1_bit(%s, %u) = %u. Expected %u. i = %u\n",
                    test[i].num, test[i].pos_start, bit_pos, test[i].bit1_pos, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(a);
        printf("end of big_int_scan0_bit() & big_int_scan1_bit() tests\n");
    }
    printf("\n");
}

/**
    Test of number theory funcs
*/
void test_number_theory(void)
{
    /* big_int_gcd() & big_int_gcd_extended() */
    {
        struct {
            char *n1;
            char *n2;
            char *gcd;
        } test[] = {
            {"1", "10", "1"},
            {"-123213123879350", "-3249283423126", "2"},
            {"12389123890", "-1523862238470", "12389123890"},
            {"-8193465725814765556554001028792218867", "5756130429098929077956071497934208671", "1"},
        };
        int cmp_flag;
        big_int_str *str = NULL;
        big_int *a = NULL, *b = NULL, *c = NULL, *x = NULL, *y = NULL;
        size_t i;

        printf("test of big_int_gcd & big_int_gcd_extended...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        c = big_int_create(1);
        x = big_int_create(1);
        y = big_int_create(1);
        if (a == NULL || b == NULL || c == NULL || x == NULL || y == NULL) {
            debug_print("error when creating numbers [a] or [b]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].n1, strlen(test[i].n1), str)) {
                debug_print("error when copying string [%s] to [str]. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string [%s] to number [a]. (i = %u)\n",
                    test[i].n1, i);
            }
            if (big_int_str_copy_s(test[i].n2, strlen(test[i].n2), str)) {
                debug_print("error when copying string [%s] to [str]. (i = %u)\n", test[i].n2, i);
            }
            if (big_int_from_str(str, 10, b)) {
                debug_print("error when converting string [%s] to number [b]. (i = %u)\n",
                    test[i].n2, i);
            }
            /* test big_int_gcd */
            if (big_int_gcd(a, b, c)) {
                debug_print("error in big_int_gcd(%s, %s). (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (big_int_to_str(c, 10, str)) {
                debug_print("error when convering big_int_gcd(%s, %s) to string [str]. (i = %u)\n",
                    test[i].n1, test[i].n2, i);
            }
            if (strcmp(str->str, test[i].gcd)) {
                debug_print("wrong result of big_int_gcd(%s, %s) = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, str->str, test[i].gcd, i);
            }
            /* calculate absoulte values of [a] and [b] */
            if (big_int_abs(a, a)) {
                debug_print("error in big_int_abs(%s) for [a]. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_abs(b, b)) {
                debug_print("error in big_int_abs(%s) for [b]. (i = %u)\n", test[i].n2, i);
            }
            /* test big_int_gcd_extended */
            if (big_int_gcd_extended(a, b, c, x, y)) {
                debug_print("error in big_int_gcd_extended(%s, %s). (i = %u)\n",
                    test[i].n1, test[i].n2, i);
            }
            if (big_int_to_str(c, 10, str)) {
                debug_print("error when convering big_int_gcd_extended(%s, %s) to string [str]. (i = %u)\n",
                    test[i].n1, test[i].n2, i);
            }
            if (strcmp(str->str, test[i].gcd)) {
                debug_print("wrong result of big_int_gcd_extended(%s, %s) = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, str->str, test[i].gcd, i);
            }
            /* check x * abs(a) + y * abs(b) = c */
            if (big_int_mul(x, a, a)) {
                debug_print("error in big_int_mul(x, %s). (i = %u)\n", test[i].n1, i);
            }
            if (big_int_mul(y, b, b)) {
                debug_print("error in big_int_mul(y, %s). (i = %u)\n", test[i].n2, i);
            }
            if (big_int_add(a, b, a)) {
                debug_print("error in big_int_add(x * %s, y * %s). (i = %u)\n",
                    test[i].n1, test[i].n2, i);
            }
            if (big_int_to_str(a, 10, str)) {
                debug_print("error when convering (x * %s + y * %s) to string [str]. (i = %u)\n",
                    test[i].n1, test[i].n2, i);
            }
            big_int_cmp(a, c, &cmp_flag);
            if (cmp_flag != 0) {
                debug_print("wrong result of x * abs(%s) + y * abs(%s) = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, str->str, test[i].gcd, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(y);
        big_int_destroy(x);
        big_int_destroy(c);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_gcd & big_int_gcd_extended test\n");
    }
    printf("\n");

    /* big_int_sqrt() & big_int_sqrt_rem() */
    {
        struct {
            char *num;
            char *sqrt;
            char *rem;
        } test[] = {
            {"0", "0", "0"},
            {"25", "5", "0"},
            {"123893478943987", "11130744", "16950451"},
            {"48820873897913044424360911842448355626", "6987193563793194675", "1"},
            {"95669864651206290465406058727876330496", "9781097313246928864", "0"},
        };
        big_int_str *str = NULL;
        big_int *a = NULL, *b = NULL;
        size_t i;

        printf("big_int_sqrt & big_int_sqrt_rem tests...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        if (a == NULL || b == NULL) {
            debug_print("error when creating number [a]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. (i = %u)\n", test[i].num, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string [%s] to number [a]. (i = %u)\n", test[i].num, i);
            }
            /* test big_int_sqrt() */
            if (big_int_sqrt(a, b)) {
                debug_print("error in big_int_sqrt(%s). (i = %u)\n", test[i].num, i);
            }
            if (big_int_to_str(b, 10, str)) {
                debug_print("error when converting big_int_sqrt(%s) to string. (i = %u)\n", test[i].num, i);
            }
            if (strcmp(str->str, test[i].sqrt)) {
                debug_print("wrong result of big_int_sqrt(%s) = %s. Expected %s. (i = %u)\n",
                    test[i].num, str->str, test[i].sqrt, i);
            }
            /* test big_int_sqrt_rem() */
            if (big_int_sqrt_rem(a, a)) {
                debug_print("error in big_int_sqrt_rem(%s). (i = %u)\n", test[i].num, i);
            }
            if (big_int_to_str(a, 10, str)) {
                debug_print("error when converting big_int_sqrt_rem(%s) to string. (i = %u)\n", test[i].num, i);
            }
            if (strcmp(str->str, test[i].rem)) {
                debug_print("wrong result of big_int_sqrt_rem(%s) = %s. Expected %s. (i = %u)\n",
                    test[i].num, str->str, test[i].rem, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_sqrt & big_int_sqrt_rem tests\n");
    }
    printf("\n");

    /* big_int_pow() */
    {
        struct {
            char *num;
            int pow;
            char *ans;
        } test[] = {
            {"0", 0, "0"},
            {"0", 100, "0"},
            {"10023423432489", 0, "1"},
            {"-5", 100, "7888609052210118054117285652827862296732064351090230047702789306640625"},
            {"2", 128, "340282366920938463463374607431768211456"},
            {"-17", 21, "-69091933913008732880827217"},
            {"123213123", -100, "0"}, /* a^b=0, if b < 0 */
        };
        big_int_str *str = NULL;
        big_int *a = NULL;
        size_t i;

        printf("big_int_pow test...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("error when creating number [a]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. i = %u\n", test[i].num, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string [%s] to number [a]. i = %u\n", str->str, i);
            }
            if (big_int_pow(a, test[i].pow, a)) {
                debug_print("error in big_int_pow(%s, %d). i = %u\n", test[i].num, test[i].pow, i);
            }
            if (big_int_to_str(a, 10, str)) {
                debug_print("error when converting number big_int_pow(%s, %d) to string [str]. i = %u\n",
                    test[i].num, test[i].pow, i);
            }
            if (strcmp(str->str, test[i].ans)) {
                debug_print("wrong result of big_int_pow(%s, %d) = %s. Expected %s. i = %u\n",
                    test[i].num, test[i].pow, str->str, test[i].ans, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(a);
        printf("end of big_int_pow test\n");
    }
    printf("\n");

    /* big_int_fact() */
    {
        struct {
            int num;
            char *fact;
        } test[] = {
            {0, "1"},
            {1, "1"},
            {12, "479001600"},
            {50, "30414093201713378043612608166064768844377641568960512000000000000"},
            {70, "11978571669969891796072783721689098736458938142546425857555362864628009582789845319680000000000000000"},
        };
        big_int_str *str = NULL;
        big_int *a = NULL;
        size_t i;

        printf("big_int_fact test...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("error when creating number [a]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_fact(test[i].num, a)) {
                debug_print("error in big_int_fact(%d). i = %u\n", test[i].num, i);
            }
            if (big_int_to_str(a, 10, str)) {
                debug_print("error when converting number big_int_fact(%d) to string [str]. i = %u\n",
                    test[i].num, i);
            }
            if (strcmp(str->str, test[i].fact)) {
                debug_print("wrong result of big_int_fact(%d) = %s. Expected %s. i = %u\n",
                    test[i].num, str->str, test[i].fact, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(a);
        printf("end of big_int_fact test\n");
    }
    printf("\n");

    /* big_int_miller_test() */
    {
        struct {
            char *num;
            char *base;
            int is_prime;
        } test[] = {
            {"2047", "2", 1}, /* 2047 is first pseudoprime by base 2 */
            {"341550071728321", "17", 1},
            {"341550071728321", "5", 1},
            {"341550071728321", "11", 1},
            {"341550071728321", "23", 0},
            {"25", "7", 1},
            {"25", "2", 0},
        };
        big_int_str *str = NULL;
        big_int *a = NULL, *b = NULL;
        size_t i;
        int is_prime;

        printf("big_int_miller_test test...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        if (a == NULL || b == NULL) {
            debug_print("error when creating numbers [a] of [b]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating string [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. i = %u\n", test[i].num, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string [%s] to number [a]. i = %u\n", test[i].num, i);
            }
            if (big_int_str_copy_s(test[i].base, strlen(test[i].base), str)) {
                debug_print("error when copying string [%s] to [str]. i = %u\n", test[i].base, i);
            }
            if (big_int_from_str(str, 10, b)) {
                debug_print("error when converting string [%s] to number [b]. i = %u\n", test[i].base, i);
            }
            if (big_int_miller_test(a, b, &is_prime)) {
                debug_print("error in big_int_miller_test(%s, %s). i = %u\n", test[i].num, test[i].base, i);
            }
            if (is_prime != test[i].is_prime) {
                debug_print("wrong result of big_int_miller_test(%s, %s) = %d. Expected %d. i = %u\n",
                    test[i].num, test[i].base, is_prime, test[i].is_prime, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_miller_test test\n");
    }
    printf("\n");

    /* test big_int_is_prime() */
    {
        struct {
            char *num;
            unsigned int primes_to;
            int level;
            int is_prime;
        } test[] = {
            {"0", 10, 0, 0},
            {"1", 10, 0, 0},
            {"2", 10, 0, 2},
            {"205891132094653", 100, 0, 1},
            {"205891132094653", 100, 1, 2},
            {"6366805760909027985741435139224233", 100, 1, 1},
            {"6366805760909027985741435139224231", 100, 0, 0},
            {"100000000000000000039", 10, 2, 1},
        };
        big_int_str *str = NULL;
        big_int *a = NULL;
        size_t i;
        int is_prime;

        printf("big_int_is_prime test...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("error when creating number [a]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. i = %u\n", test[i].num, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string [%s] to number [a]. i = %u\n",
                    test[i].num, i);
            }
            if (big_int_is_prime(a, test[i].primes_to, test[i].level, &is_prime)) {
                debug_print("error in big_int_is_prime(%s, %u, %d). i = %u\n",
                    test[i].num, test[i].primes_to, test[i].level);
            }
            if (is_prime != test[i].is_prime) {
                debug_print("wrong result of big_int_is_prime(%s, %u, %d) = %d. Expected %d. i = %u\n",
                    test[i].num, test[i].primes_to, test[i].level, is_prime, test[i].is_prime, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(a);
        printf("end of big_int_is_prime test\n");
    }
    printf("\n");

    /* big_int_next_prime() */
    {
        struct {
            char *num;
            char *next_prime;
        } test[] = {
            {"0", "2"},
            {"100000000000000", "100000000000031"},
            {"-100000000000000", "-99999999999973"},
            {"671790528819082282036142601601", "671790528819082282036142601623"},
            {"8177392779424694984166105409421310233329973193707154566120313244275803525481497595419852266351308626", "8177392779424694984166105409421310233329973193707154566120313244275803525481497595419852266351308743"},
        };
        big_int_str *str = NULL;
        big_int *a = NULL;
        size_t i;

        printf("big_int_next_prime test...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("error when creating number [a]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. i = %u\n", test[i].num, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string [%s] into number [a]. i = %u\n",
                    test[i].num, i);
            }
            if (big_int_next_prime(a, a)) {
                debug_print("error in big_int_next_prime(%s). i = %u\n", test[i].num, i);
            }
            if (big_int_to_str(a, 10, str)) {
                debug_print("error when converting number big_int_next_prime(%s) to string [str]. i = %u\n",
                    test[i].num, i);
            }
            if (strcmp(str->str, test[i].next_prime)) {
                debug_print("wrong result of big_int_next_prime(%s) = %s. Expected %s. i = %u\n",
                    test[i].num, str->str, test[i].next_prime);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(a);
        printf("end of big_int_next_prime test\n");
    }
    printf("\n");

    /* big_int_jacobi() */
    {
        struct {
            char *n1;
            char *n2;
            int jacobi;
        } test[] = {
            {"10", "21", -1},
            {"20", "31", 1},
        };
        size_t i;
        int jacobi;
        big_int_str *str = NULL;
        big_int *a = NULL, *b = NULL;

        printf("big_int_jacobi test...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        if (a == NULL || b == NULL) {
            debug_print("error when creating numbers [a] or [b]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].n1, strlen(test[i].n1), str)) {
                debug_print("error when copying string [%s] to [str]. i = %u\n", test[i].n1, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string [%s] to number [a]. i = %u\n", str->str, i);
            }
            if (big_int_str_copy_s(test[i].n2, strlen(test[i].n2), str)) {
                debug_print("error when copying string [%s] to [str]. i = %u\n", test[i].n2, i);
            }
            if (big_int_from_str(str, 10, b)) {
                debug_print("error when converting string [%s] to number [b]. i = %u\n", str->str, i);
            }
            if (big_int_jacobi(a, b, &jacobi)) {
                debug_print("error in big_int_jacobi(%s, %s). i = %u\n", test[i].n1, test[i].n2, i);
            }
            if (jacobi != test[i].jacobi) {
                debug_print("wrong result of big_int_jacobi(%s, %s) = %d. Expected %d. i = %u\n",
                    test[i].n1, test[i].n2, jacobi, test[i].jacobi, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_jacobi test\n");
    }
    printf("\n");
}

/**
    Test of modular arithmetic funcs
*/
void test_modular_arithmetic(void)
{
    /* big_int_absmod(), big_int_sqrmod() */
    {
        struct {
            char *num;
            char *modulus;
            char *abs;
            char *sqr;
        } test[] = {
            {"0", "123432423", "0", "0"},
            {"21213231", "1", "0", "0"},
            {"-5", "3", "1", "1"},
            {"5", "-3", "2", "1"},
            {"-4", "7", "3", "2"},
            {"219039023334890932478932446347893247823423784237", "328934789324763243243434", "108718213483860266487771", "4044336053565329527131"},
            {"1267650600228229401496703205653", "633825300114114700748351602943", "633825300114114700748351602710", "54289"},
        };
        big_int *a = NULL, *b = NULL, *c = NULL;
        big_int_str *str = NULL;
        size_t i;

        printf("big_int_absmod & big_int_sqrmod test...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        c = big_int_create(1);
        if (a == NULL || b == NULL || c == NULL) {
            debug_print("error when creating [a], [b] or [c] numbers\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating big_int_str object [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] to [str]. (i = %u)\n", test[i].num, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string [%s] to number [a]. (i = %u)\n", test[i].num, i);
            }
            if (big_int_str_copy_s(test[i].modulus, strlen(test[i].modulus), str)) {
                debug_print("error when copying string [%s] to [str]. (i = %u)\n", test[i].modulus, i);
            }
            if (big_int_from_str(str, 10, b)) {
                debug_print("error when converting string [%s] to number [b]. (i = %u)\n", test[i].modulus, i);
            }
            /* test big_int_absmod() */
            if (big_int_absmod(a, b, c)) {
                debug_print("error in big_int_absmod(%s, %s, c).  (i = %u)\n", test[i].num, test[i].modulus, i);
            }
            if (big_int_to_str(c, 10, str)) {
                debug_print("error when converting number [c] to string after big_int_absmod. (i = %u)\n", i);
            }
            if (strcmp(str->str, test[i].abs)) {
                debug_print("wrong result after big_int_absmod(%s, %s): %s. Expected %s. (i = %u)\n",
                    test[i].num, test[i].modulus, str->str, test[i].abs, i);
            }
            /* test big_int_sqrmod() */
            if (big_int_sqrmod(a, b, c)) {
                debug_print("error in big_int_absmod(%s, %s, c).  (i = %u)\n", test[i].num, test[i].modulus, i);
            }
            if (big_int_to_str(c, 10, str)) {
                debug_print("error when converting number [c] to string after big_int_sqrmod. (i = %u)\n", i);
            }
            if (strcmp(str->str, test[i].sqr)) {
                debug_print("wrong result after big_int_absmod(%s, %s): %s. Expected %s. (i = %u)\n",
                    test[i].num, test[i].modulus, str->str, test[i].sqr, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(c);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_absmod & big_int_sqrmod test\n");
    }
    printf("\n");

    /* big_int_invmod() */
    {
        struct {
            char *num;
            char *modulus;
            char *inv;
        } test[] = {
            {"-5", "18446744073709551616", "3689348814741910323"},
            {"3", "18446744073709551616", "12297829382473034411"},
            {"332489342783427", "18446744073709551616", "17048097163862923499"},
            {"3427892347891237892138970123", "347832478123892312317623178123721378", "68050276522879349900817103954877941"},
        };
        big_int *a = NULL, *b = NULL;
        big_int_str *str = NULL;
        size_t i;

        printf("big_int_invmod test...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        if (a == NULL || b == NULL) {
            debug_print("cannot create [a] or [b] numbers\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("cannot create [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string [%s] (num) to [str]. (i = %u)\n", test[i].num, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string [%s] to number [a]. (i = %u)\n", test[i].num, i);
            }
            if (big_int_str_copy_s(test[i].modulus, strlen(test[i].modulus), str)) {
                debug_print("error when copying string [%s] (modulus) to [str]. (i = %u)\n", test[i].modulus, i);
            }
            if (big_int_from_str(str, 10, b)) {
                debug_print("error when converting string [%s] to number [b]. (i = %u)\n", test[i].modulus, i);
            }
            if (big_int_invmod(a, b, a)) {
                debug_print("error in big_int_invmod(%s, %s, a). (i = %u)\n", test[i].num, test[i].modulus, i);
            }
            if (big_int_to_str(a, 10, str)) {
                debug_print("error when converting number [a] to string. (i = %u)\n", i);
            }
            if (strcmp(str->str, test[i].inv)) {
                debug_print("wrong value of big_int_invmod(%s, %s) = %s. Expected %s. (i = %u)\n",
                    test[i].num, test[i].modulus, str->str, test[i].inv);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_invmod test\n");

    }
    printf("\n");

    /* big_int_factmod() */
    {
        struct {
            int num;
            char *modulus;
            char *fact;
        } test[] = {
            {0, "232323", "1"},
            {1, "34324235", "1"},
            {5, "123456", "120"},
            {1000, "18446744073709551629", "8075331241715570358"},
            {347, "18446744073709551629", "5568839405920862717"},
        };
        big_int *a = NULL, *b = NULL;
        big_int_str *str = NULL;
        size_t i;

        printf("big_int_factmod test...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        if (a == NULL || b == NULL) {
            debug_print("cannot create [a] or [b] numbers\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("cannot create [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_from_int(test[i].num, a)) {
                debug_print("error in big_int_from_int for number %d. (i = %u)\n", test[i].num, i);
            }
            if (big_int_str_copy_s(test[i].modulus, strlen(test[i].modulus), str)) {
                debug_print("error when copying string [%s] (modulus) to [str]. (i = %u)\n", test[i].modulus, i);
            }
            if (big_int_from_str(str, 10, b)) {
                debug_print("error when converting string [%s] to number [b]. (i = %u)\n", test[i].modulus, i);
            }
            if (big_int_factmod(a, b, b)) {
                debug_print("error in big_int_factmod(%d, %s, b). (i = %u)\n", test[i].num, test[i].modulus, i);
            }
            if (big_int_to_str(b, 10, str)) {
                debug_print("error when converting number [b] to string. (i = %u)\n", i);
            }
            if (strcmp(str->str, test[i].fact)) {
                debug_print("wrong value of big_int_factmod(%d, %s) = %s. Expected %s. (i = %u)\n",
                    test[i].num, test[i].modulus, str->str, test[i].fact, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_factmod test\n");
    }
    printf("\n");

    /* big_int_cmpmod() */
    {
        struct {
            char *n1;
            char *n2;
            char *modulus;
            int cmp_flag;
        } test[] = {
            {"0", "0", "12323", 0},
            {"213213", "123434324", "1", 0},
            {"10", "6", "7", -1},
            {"-1", "100", "324324234", 1},
        };
        big_int *a = NULL, *b = NULL, *c = NULL;
        big_int_str *str = NULL;
        size_t i;
        int cmp_flag;

        printf("big_int_cmpmod test...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        c = big_int_create(1);
        if (a == NULL || b == NULL || c == NULL) {
            debug_print("error when creating [a] or [b]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creatings big_int_str [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].n1, strlen(test[i].n1), str)) {
                debug_print("error when converting string [%s] to [str]. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string [%s] to number [a]. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_str_copy_s(test[i].n2, strlen(test[i].n2), str)) {
                debug_print("error when converting string [%s] to [str]. (i = %u)\n", test[i].n2, i);
            }
            if (big_int_from_str(str, 10, b)) {
                debug_print("error when converting string [%s] to number [b]. (i = %u)\n", test[i].n2, i);
            }
            if (big_int_str_copy_s(test[i].modulus, strlen(test[i].modulus), str)) {
                debug_print("error when converting string [%s] to [str]. (i = %u)\n", test[i].modulus, i);
            }
            if (big_int_from_str(str, 10, c)) {
                debug_print("error when converting string [%s] to number [c]. (i = %u)\n", test[i].modulus, i);
            }
            if (big_int_cmpmod(a, b, c, &cmp_flag)) {
                debug_print("error in big_int_cmpmod(%s, %s, %s). (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].modulus, i);
            }
            if (cmp_flag != test[i].cmp_flag) {
                debug_print("wrong result of big_int_cmpmod(%s, %s, %s) = %d. Expected %d. (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].modulus, cmp_flag, test[i].cmp_flag, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(c);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_cmpmod test\n");
    }
    printf("\n");

    /* big_int_addmod(), big_int_submod(), big_int_divmod(), big_int_mulmod() & big_int_powmod() */
    {
        struct {
            char *n1;
            char *n2;
            char *modulus;
            char *add;
            char *sub;
            char *div;
            char *mul;
            char *pow;
        } test[] = {
            {"0", "1", "10", "1", "9", "0", "0", "0"},
            {"1234567890", "987654321", "18446744073709551616", "2222222211", "246913569", "8993027879916446834", "1219326311126352690", "0"},
            {"-5", "-10", "717897987691852588770277", "717897987691852588770262", "5", "358948993845926294385139", "50", "395813765371433154522332"},
        };
        big_int *a = NULL, *b = NULL, *c = NULL, *d = NULL;
        big_int_str *str = NULL;
        size_t i;

        printf("test of big_int_addmod, big_int_submod, big_int_divmod, big_int_mulmod & big_int_powmod...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        c = big_int_create(1);
        d = big_int_create(1);
        if (a == NULL || b == NULL || c == NULL) {
            debug_print("error when creating [a], [b] or [c]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].n1, strlen(test[i].n1), str)) {
                debug_print("error when copying string [%s] to [str]. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting n1 = [%s] to number [a]. (i = %u)\n", str->str, i);
            }
            if (big_int_str_copy_s(test[i].n2, strlen(test[i].n2), str)) {
                debug_print("error when copying string [%s] to [str]. (i = %u)\n", test[i].n2, i);
            }
            if (big_int_from_str(str, 10, b)) {
                debug_print("error when converting n2 = [%s] to number [b]. (i = %u)\n", str->str, i);
            }
            if (big_int_str_copy_s(test[i].modulus, strlen(test[i].modulus), str)) {
                debug_print("error when copying string [%s] to [str]. (i = %u)\n", test[i].modulus, i);
            }
            if (big_int_from_str(str, 10, c)) {
                debug_print("error when converting modulus = [%s] to number [c]. (i = %u)\n", str->str, i);
            }
            /* test big_int_addmod() */
            if (big_int_addmod(a, b, c, d)) {
                debug_print("error in big_int_addmod(%s, %s, %s). (i = %u)\n", test[i].n1, test[i].n2, test[i].modulus, i);
            }
            if (big_int_to_str(d, 10, str)) {
                debug_print("error when converting number big_int_addmod(%s, %s, %s) to string [str]. (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].modulus, i);
            }
            if (strcmp(str->str, test[i].add)) {
                debug_print("wrong result of big_int_addmod(%s, %s, %s) = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].modulus, str->str, test[i].add, i);
            }
            /* test big_int_submod() */
            if (big_int_submod(a, b, c, d)) {
                debug_print("error in big_int_submod(%s, %s, %s). (i = %u)\n", test[i].n1, test[i].n2, test[i].modulus, i);
            }
            if (big_int_to_str(d, 10, str)) {
                debug_print("error when converting number big_int_submod(%s, %s, %s) to string [str]. (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].modulus, i);
            }
            if (strcmp(str->str, test[i].sub)) {
                debug_print("wrong result of big_int_submod(%s, %s, %s) = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].modulus, str->str, test[i].sub, i);
            }
            /* test big_int_divmod() */
            if (big_int_divmod(a, b, c, d)) {
                debug_print("error in big_int_divmod(%s, %s, %s). (i = %u)\n", test[i].n1, test[i].n2, test[i].modulus, i);
            }
            if (big_int_to_str(d, 10, str)) {
                debug_print("error when converting number big_int_divmod(%s, %s, %s) to string [str]. (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].modulus, i);
            }
            if (strcmp(str->str, test[i].div)) {
                debug_print("wrong result of big_int_divmod(%s, %s, %s) = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].modulus, str->str, test[i].div, i);
            }
            /* test big_int_mulmod() */
            if (big_int_mulmod(a, b, c, d)) {
                debug_print("error in big_int_mulmod(%s, %s, %s). (i = %u)\n", test[i].n1, test[i].n2, test[i].modulus, i);
            }
            if (big_int_to_str(d, 10, str)) {
                debug_print("error when converting number big_int_mulmod(%s, %s, %s) to string [str]. (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].modulus, i);
            }
            if (strcmp(str->str, test[i].mul)) {
                debug_print("wrong result of big_int_mulmod(%s, %s, %s) = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].modulus, str->str, test[i].mul, i);
            }
            /* test big_int_powmod() */
            if (big_int_powmod(a, b, c, d)) {
                debug_print("error in big_int_powmod(%s, %s, %s). (i = %u)\n", test[i].n1, test[i].n2, test[i].modulus, i);
            }
            if (big_int_to_str(d, 10, str)) {
                debug_print("error when converting number big_int_powmod(%s, %s, %s) to string [str]. (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].modulus, i);
            }
            if (strcmp(str->str, test[i].pow)) {
                debug_print("wrong result of big_int_powmod(%s, %s, %s) = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].modulus, str->str, test[i].pow, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(d);
        big_int_destroy(c);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of test of big_int_addmod, big_int_submod, big_int_divmod, big_int_mulmod & big_int_powmod\n");
    }
    printf("\n");
}

/**
    Test of string funcs
*/
void test_str_funcs(void)
{
    /*
        big_int_str_create(), big_int_str_dup(), big_int_str_copy(),
        big_int_str_copy_s(), big_int_str_realloc(), big_int_str_destroy()
    */
    struct {
        char *str;
        size_t len;
    } test[] = {
        {"", 0},
        {"qwe\0rtu", 7},
        {"1", 1},
    };
    size_t i;
    big_int_str *str1 = NULL, *str2 = NULL, *str3 = NULL;

    printf("big_int_str_* tests...\n");
    str1 = big_int_str_create(1);
    str3 = big_int_str_create(1);
    if (str1 == NULL || str3 == NULL) {
        debug_print("error when creating big_int_str object\n");
    }
    if (big_int_str_realloc(str1, 1000)) {
        debug_print("error when reallocating [str1]\n");
    }
    for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
        if (big_int_str_copy_s(test[i].str, test[i].len, str1)) {
            debug_print("error when copying string [%s] with length = %u to big_int_str object. (i = %u)\n",
                test[i].str, test[i].len, i);
        }
        /* test big_int_str_dup() */
        str2 = big_int_str_dup(str1);
        if (str2 == NULL) {
            debug_print("cannot duplicate string [%s] with length = %u. (i = %u)\n",
                test[i].str, test[i].len, i);
        }
        if (memcmp(str1->str, str2->str, str2->len)) {
            debug_print("wrong copy of string [str1] [%s] with length = %u to [str2] [%s] with length = %u (after big_int_str_dup). (i = %u)\n",
                str1->str, str1->len, str2->str, str2->len, i);
        }
        /* test big_int_str_copy() */
        if (big_int_str_copy(str1, str3)) {
            debug_print("error in big_int_str_copy() function for string [%s] with length = %u. (i = %u)\n",
                str1->str, str1->len, i);
        }
        if (memcmp(str1->str, str3->str, str3->len)) {
            debug_print("wrong copy of string [str1] [%s] with length = %u to [str3] [%s] with length = %u (after big_int_str_copy). (i = %u)\n",
                str1->str, str1->len, str3->str, str3->len, i);
        }
        big_int_str_destroy(str2);
    }
    big_int_str_destroy(str3);
    big_int_str_destroy(str1);
    printf("end of big_int_str_* tests\n");
    printf("\n");
}

/**
    Test of basic funcs
*/
void test_basic_funcs(void)
{
    /* big_int_abs(), big_int_cmp_abs() & big_int_cmp() */
    {
        size_t i;
        big_int *a = NULL, *b = NULL;
        struct {
            char *n1;
            char *n2;
            int cmp_abs;
            int cmp;
        } test[] = {
            {"-1234567890", "0", 1, -1},
            {"0", "0", 0, 0},
            {"21903209239023190", "21903209239023190", 0, 0},
            {"21903209239023190", "-21903209239023190", 0, 1},
            {"-21903209239023190", "21903209239023191", -1, -1},
            {"21903209239023190", "-21903209239023191", -1, 1},
        };
        big_int_str *str = NULL;
        int cmp_flag;

        printf("big_int_abs, big_int_cmp_abs & big_int_cmp tests...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        if (a == NULL || b == NULL) {
            debug_print("error when creating big_int numbers [a] or [b]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating big_int_str string\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].n1, strlen(test[i].n1), str)) {
                debug_print("error when copying string %s to big_int_str. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string %s to number [a]. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_str_copy_s(test[i].n2, strlen(test[i].n2), str)) {
                debug_print("error when copying string %s to big_int_str. (i = %u)\n", test[i].n2, i);
            }
            if (big_int_from_str(str, 10, b)) {
                debug_print("error when converting string %s to number [b]. (i = %u)\n", test[i].n2, i);
            }
            big_int_cmp(a, b, &cmp_flag);
            if (cmp_flag != test[i].cmp) {
                debug_print("wrong comparision of numbers %s and %s. Cmp_flag = %d, expected %d (i = %u)\n",
                    test[i].n1, test[i].n2, cmp_flag, test[i].cmp, i);
            }
            big_int_cmp_abs(a, b, &cmp_flag);
            if (cmp_flag != test[i].cmp_abs) {
                debug_print("wrong abs comparision of numbers %s and %s. Cmp_flag = %d, expected %d (i = %u)\n",
                    test[i].n1, test[i].n2, cmp_flag, test[i].cmp, i);
            }
            /* manually abs [a] and [b] */
            if (big_int_abs(a, a)) {
                debug_print("error in abs() number %s. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_abs(b, b)) {
                debug_print("error in abs() number %s. (i = %u)\n", test[i].n2, i);
            }
            big_int_cmp(a, b, &cmp_flag);
            if (cmp_flag != test[i].cmp_abs) {
                debug_print("wrong abs comparision after abs(a), abs(b) of numbers %s and %s. Cmp_flag = %d, expected %d (i = %u)\n",
                    test[i].n1, test[i].n2, cmp_flag, test[i].cmp, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_abs, big_int_cmp_abs & big_int_cmp tests\n");
    }
    printf("\n");

    /* big_int_sign() & big_int_neg() */
    {
        big_int a;
        sign_type sign;
        big_int_word num;

        printf("big_int_sign & big_int_neg test...\n");
        num = 10;
        a.len = 1;
        a.num = &num;
        a.sign = PLUS;
        big_int_sign(&a, &sign);
        if (sign != PLUS) {
            debug_print("wrong sign: MINUS. Expected PLUS\n");
        }
        a.sign = MINUS;
        big_int_sign(&a, &sign);
        if (sign != MINUS) {
            debug_print("wrong sign: PLUS. Expected MINUS\n");
        }
        if (big_int_neg(&a, &a)) {
            debug_print("error in big_int_neg function\n");
        }
        big_int_sign(&a, &sign);
        if (sign != PLUS) {
            debug_print("wrong sign after big_int_neg: MINUS. Expected PLUS\n");
        }
        /* try to invert zero (-0 = +0) */
        num = 0;
        a.sign = PLUS;
        if (big_int_neg(&a, &a)) {
            debug_print("error in big_int_neg function for zero\n");
        }
        big_int_sign(&a, &sign);
        if (sign != PLUS) {
            debug_print("wrong sign for zero: MINUS. Expected PLUS\n");
        }
        printf("end of big_int_sign test\n");
    }
    printf("\n");

    /* big_int_inc() & big_int_dec() */
    {
        big_int *a = NULL, *b = NULL;
        big_int_str *str = NULL;
        char *nums[] = {
            "0",
            "-1",
            "1",
            "256",
            "+65536",
            "4294967296",
            "18446744073709551616",
            "255",
            "65535",
            "4294967295",
            "18446744073709551615",
        };
        size_t i;
        int cmp_flag;

        printf("big_int_inc & big_int_dec tests...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        if (a == NULL || b == NULL) {
            debug_print("error when creating [a] or [b] number\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating big_int_str [str]\n");
        }
        for (i = 0; i < sizeof(nums) / sizeof(nums[0]); i++) {
            if (big_int_str_copy_s(nums[i], strlen(nums[i]), str)) {
                debug_print("error when copying string %s to big_int_str variable [str]. (i = %u)\n", nums[i], i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string %s to number [a]. (i = %u)\n", nums[i], i);
            }
            if (big_int_inc(a, b)) {
                debug_print("error in big_int_inc() function for number %s. (i = %u)\n", nums[i], i);
            }
            if (big_int_dec(b, b)) {
                debug_print("error in big_int_dec() function for number %s + 1. (i = %u)\n", nums[i], i);
            }
            big_int_cmp_abs(a, b, &cmp_flag);
            if (cmp_flag != 0) {
                if (big_int_to_str(b, 10, str)) {
                    debug_print("[inc, dec]. error when converting [b] to string. (i = %u)\n", i);
                }
                debug_print("wrong result after [inc, dec] calls. Current value: %s, Expected %s. (i = %u)\n",
                    str->str, nums[i], i);
            }
            if (big_int_dec(b, b)) {
                debug_print("error in big_int_dec() function for number %s. (i = %u)\n", nums[i], i);
            }
            if (big_int_inc(b, b)) {
                debug_print("error in big_int_inc() function for number %s - 1. (i = %u)\n", nums[i], i);
            }
            big_int_cmp_abs(a, b, &cmp_flag);
            if (cmp_flag != 0) {
                if (big_int_to_str(b, 10, str)) {
                    debug_print("[dec, inc]. error when converting [b] to string. (i = %u)\n", i);
                }
                debug_print("wrong result after [dec, inc] calls. Current value: %s, Expected %s. (i = %u)\n",
                    str->str, nums[i], i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_inc & big_int_dec tests\n");
    }
    printf("\n");

    /* big_int_sqr */
    {
        struct {
            char *num;
            char *sqr_num;
        } test[] = {
            {"0", "0"},
            {"-1", "1"},
            {"34890324890234089", "1217334770946088394626405217659921"},
            {"-342892389238972347854458923423423478", "117575190598010919578616672149123988281495502443041599148875829721616484"},
        };
        big_int_str *str = NULL;
        big_int *a = NULL;
        size_t i;

        printf("big_int_sqr test...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("error when creating [a] number\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating big_int_str [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].num, strlen(test[i].num), str)) {
                debug_print("error when copying string %s to big_int_str variable [str]. (i = %u)\n", test[i].num, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string %s to number [a]. (i = %u)\n", test[i].num, i);
            }
            if (big_int_sqr(a, a)) {
                debug_print("error in big_int_sqr() function for number %s. (i = %u)\n", test[i].num, i);
            }
            if (big_int_to_str(a, 10, str)) {
                debug_print("error when converting number %s^2 to string [str]. (i = %u)\n", test[i].num, i);
            }
            if (strcmp(str->str, test[i].sqr_num)) {
                debug_print("wrong result of big_int_sqr(%s): %s. Expected %s. (i = %u)\n",
                    test[i].num, str->str, test[i].sqr_num);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(a);
        printf("end of big_int_sqr test\n");
    }
    printf("\n");

    /* big_int_add(), big_int_sub() & big_int_mul() */
    {
        struct {
            char *n1;
            char *n2;
            char *sum;
            char *dif;
            char *mul;
        } test[] = {
            {"0", "0", "0", "0", "0"},
            {"0", "1", "1", "-1", "0"},
            {"-1", "1", "0", "-2", "-1"},
            {"255", "-1", "254", "256", "-255"},
            {"18446744073709551616", "1", "18446744073709551617", "18446744073709551615", "18446744073709551616"},
            {"18446744073709551616", "18446744073709551615", "36893488147419103231", "1", "340282366920938463444927863358058659840"},
        };
        size_t i;
        big_int *a = NULL, *b = NULL, *c = NULL;
        big_int_str *str = NULL;

        printf("big_int_add, big_int_sub & big_int_mul tests...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        c = big_int_create(1);
        if (a == NULL || b == NULL || c == NULL) {
            debug_print("error when creating [a], [b] or [c] number\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating big_int_str [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].n1, strlen(test[i].n1), str)) {
                debug_print("error when copying string %s to big_int_str variable [str]. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string %s to number [a]. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_str_copy_s(test[i].n2, strlen(test[i].n2), str)) {
                debug_print("error when copying string %s to big_int_str variable [str]. (i = %u)\n", test[i].n2, i);
            }
            if (big_int_from_str(str, 10, b)) {
                debug_print("error when converting string %s to number [b]. (i = %u)\n", test[i].n2, i);
            }
            /* big_int_add test */
            if (big_int_add(a, b, c)) {
                debug_print("error in big_int_add function for numbers %s + %s (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (big_int_to_str(c, 10, str)) {
                debug_print("error when converting number (%s + %s) to string. (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (strcmp(str->str, test[i].sum)) {
                debug_print("wrong result in big_int_add() call for numbers %s + %s = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, str->str, test[i].sum, i);
            }
            /* big_int_sub test */
            if (big_int_sub(a, b, c)) {
                debug_print("error in big_int_sub function for numbers %s - %s. (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (big_int_to_str(c, 10, str)) {
                debug_print("error when converting number (%s - %s) to string. (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (strcmp(str->str, test[i].dif)) {
                debug_print("wrong result in big_int_sub() call for numbers %s - %s = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, str->str, test[i].dif, i);
            }
            /* big_int_mul test */
            if (big_int_mul(a, b, c)) {
                debug_print("error in big_int_mul function for numbers %s * %s. (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (big_int_to_str(c, 10, str)) {
                debug_print("error when converting number (%s * %s) to string. (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (strcmp(str->str, test[i].mul)) {
                debug_print("wrong result in big_int_sub() call for numbers %s * %s = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, str->str, test[i].dif, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(c);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_add, big_int_sub & big_int_mul tests\n");
    }
    printf("\n");

    /* big_int_div(), big_int_mod() & big_int_div_extended() */
    {
        struct {
            char *n1;
            char *n2;
            char *div;
            char *mod;
        } test[] = {
            {"0", "1", "0", "0"},
            {"1", "-1", "-1", "0"},
            {"-65536", "1", "-65536", "0"},
            {"2344232352767889343324", "-3223439724", "-727245598952", "1839774076"},
            {"-340282366920938463463374607431768211455", "-36893488147419103363", "9223372036854775775", "-9223372036854780130"},
        };
        size_t i;
        big_int *a = NULL, *b = NULL, *c = NULL;
        big_int_str *str = NULL;

        printf("big_int_div, big_int_mod & big_int_div_extended tests...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        c = big_int_create(1);
        if (a == NULL || b == NULL || c == NULL) {
            debug_print("error when creating [a], [b] or [c] number\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating big_int_str [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].n1, strlen(test[i].n1), str)) {
                debug_print("error when copying string %s to big_int_str variable [str]. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string %s to number [a]. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_str_copy_s(test[i].n2, strlen(test[i].n2), str)) {
                debug_print("error when copying string %s to big_int_str variable [str]. (i = %u)\n", test[i].n2, i);
            }
            if (big_int_from_str(str, 10, b)) {
                debug_print("error when converting string %s to number [b]. (i = %u)\n", test[i].n2, i);
            }
            /* big_int_div test */
            if (big_int_div(a, b, c)) {
                debug_print("error in big_int_div function for numbers %s div %s (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (big_int_to_str(c, 10, str)) {
                debug_print("error when converting number (%s div %s) to string. (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (strcmp(str->str, test[i].div)) {
                debug_print("wrong result in big_int_div() call for numbers %s div %s = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, str->str, test[i].div, i);
            }
            /* big_int_mod test */
            if (big_int_mod(a, b, c)) {
                debug_print("error in big_int_sub function for numbers %s mod %s. (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (big_int_to_str(c, 10, str)) {
                debug_print("error when converting number (%s mod %s) to string. (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (strcmp(str->str, test[i].mod)) {
                debug_print("wrong result in big_int_mod() call for numbers %s mod %s = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, str->str, test[i].mod, i);
            }
            /* big_int_div_extended test */
            if (big_int_div_extended(a, b, c, a)) {
                debug_print("error in big_int_div_extended function for numbers %s , %s. (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (big_int_to_str(c, 10, str)) {
                debug_print("error when converting number (%s div %s) to string. (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (strcmp(str->str, test[i].div)) {
                debug_print("wrong result in big_int_div_extended() call for numbers %s div %s = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, str->str, test[i].div, i);
            }
            if (big_int_to_str(a, 10, str)) {
                debug_print("error when converting number (%s mod %s) to string. (i = %u)\n", test[i].n1, test[i].n2, i);
            }
            if (strcmp(str->str, test[i].mod)) {
                debug_print("wrong result in big_int_div_extended() call for numbers %s mod %s = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, str->str, test[i].div, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(c);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_div, big_int_mod & big_int_div_extended tests\n");
    }
    printf("\n");

    /* big_int_muladd() */
    {
        /* result = n3 + n1 * n2 */
        struct {
            char *n1;
            char *n2;
            char *n3;
            char *result;
        } test[] = {
            {"0", "0", "0", "0"},
            {"100", "0", "-100", "-100"},
            {"-256", "1", "1", "-255"},
            {"256", "1", "-1", "255"},
            {"-256", "-1", "1", "257"},
            {"256", "-1", "-1", "-257"},
            {"18446744073709551615", "18446744073709551615", "18446744073709551615", "340282366920938463444927863358058659840"},
        };
        size_t i;
        big_int *a = NULL, *b = NULL, *c = NULL;
        big_int_str *str = NULL;

        printf("big_int_muladd test...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        c = big_int_create(1);
        if (a == NULL || b == NULL || c == NULL) {
            debug_print("error when creating [a], [b] or [c] number\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating big_int_str [str]\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].n1, strlen(test[i].n1), str)) {
                debug_print("error when copying string %s to big_int_str variable [str]. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string %s to number [a]. (i = %u)\n", test[i].n1, i);
            }
            if (big_int_str_copy_s(test[i].n2, strlen(test[i].n2), str)) {
                debug_print("error when copying string %s to big_int_str variable [str]. (i = %u)\n", test[i].n2, i);
            }
            if (big_int_from_str(str, 10, b)) {
                debug_print("error when converting string %s to number [b]. (i = %u)\n", test[i].n2, i);
            }
            if (big_int_str_copy_s(test[i].n3, strlen(test[i].n3), str)) {
                debug_print("error when copying string %s to big_int_str variable [str]. (i = %u)\n", test[i].n3, i);
            }
            if (big_int_from_str(str, 10, c)) {
                debug_print("error when converting string %s to number [c]. (i = %u)\n", test[i].n3, i);
            }
            /* big_int_muladd test */
            if (big_int_muladd(a, b, c, a)) {
                debug_print("error in big_int_muladd function for numbers %s * %s + %s (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].n3, i);
            }
            if (big_int_to_str(a, 10, str)) {
                debug_print("error when converting number (%s * %s + %s) to string. (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].n3, i);
            }
            if (strcmp(str->str, test[i].result)) {
                debug_print("wrong result in big_int_muladd() call for numbers %s * %s + %s = %s. Expected %s. (i = %u)\n",
                    test[i].n1, test[i].n2, test[i].n3, str->str, test[i].result, i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(c);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_muladd tests\n");
    }
    printf("\n");

}

/**
    Test of service funcs
*/
void test_service_funcs(void)
{
    /* big_int_create() & big_int_destroy() */
    {
        size_t i;
        big_int *a = NULL;
        size_t len[] = {1, 2, 4, 100, 1000, 10000,}; /* array of lengthes to test */

        printf("test big_int_create & big_int_destroy...\n");
        for (i = 0; i < sizeof(len) / sizeof(size_t); i++) {
            a = big_int_create(len[i]);
            if (a == NULL) {
                debug_print("cannot create number with length = %u,  (i = %u)\n", len[i], i);
            }
            big_int_destroy(a);
        }
        printf("end of big_int_create & big_int_destroy tests\n");
    }
    printf("\n");

    /* big_int_dup(), big_int_from_int() & big_int_to_int */
    {
        size_t i;
        big_int *a = NULL, *b = NULL;
        int val[] = {0, 1, -10, -10000000, ~0, 12345, ~4567,};
        int tmp;

        printf("test big_int_dup, big_int_from_int & big_int_to_int...\n");
        a = big_int_create(1);
        if (a == NULL) {
            debug_print("cannot create number with length 1\n");
        }
        b = big_int_create(1);
        if (b == NULL) {
            debug_print("cannot create number with length 1\n");
        }
        for (i = 0; i < sizeof(val) / sizeof(int); i++) {
            if (big_int_from_int(val[i], a)) {
                debug_print("error when converting int %d to number (i = %u)\n", val[i], i);
            }
            b = big_int_dup(a);
            if (b == NULL) {
                debug_print("error when duplicating number %d (i = %u)\n", val[i], i);
            }
            if (big_int_to_int(b, &tmp)) {
                debug_print("error when converting number %d to int (i = %u)\n", val[i], i);
            }
            if (val[i] != tmp) {
                debug_print("wrong conversion for number %d != %d (i = %u)\n", val[i], tmp, i);
            }
        }
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_dup test\n");
    }
    printf("\n");

    /* big_int_clear_zeros() */
    {
        big_int *a = NULL;
        size_t i, j;
        struct {
            size_t len;
            size_t pos;
            big_int_word num;
        } test[] = {
            {1, 0, 0}, /* zero */
            {1000, 543, 1},
            {10, 9, 0}, /* zero */
            {100, 0, 1}, /* one */
            {100, 34, 1},
        };

        printf("big_int_clear_zeros test...\n");
        for (i = 0; i < sizeof(test) / sizeof(test[0]) ; i++) {
            a = big_int_create(test[i].len);
            for (j = 0; j < test[i].len; j++) {
                a->num[j] = 0;
            }
            a->len = test[i].len;
            a->num[test[i].pos] = test[i].num;
            big_int_clear_zeros(a);
            if ((test[i].num > 0 || a->len > 1) && (a->len != test[i].pos + 1)) {
                debug_print("wrong clearing zeros for i = %d (num = %u, len = %u, pos = %u). Real len = %d\n",
                    i, test[i].num, test[i].len, test[i].pos, a->len);
            }
            big_int_destroy(a);
        }
        printf("end of big_int_clear_zeros test\n");
    }
    printf("\n");

    /* big_int_copy(), big_int_from_str() & big_int_cmp() */
    {
        size_t i;
        int cmp_flag;
        big_int *a = NULL, *b = NULL;
        big_int_str *str = NULL;
        char *nums[] = {
            "0",
            "+1234567890987654321",
            "123218897778782",
            "-123456781111111111112323",
            "-0",
        };

        printf("big_int_copy, big_int_from_str & big_int_cmp tests...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        if (a == NULL || b == NULL) {
            debug_print("error when creating numbers [a] or [b]\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating big_int_str string\n");
        }
        for (i = 0; i < sizeof(nums) / sizeof(nums[0]); i++) {
            if (big_int_str_copy_s(nums[i], strlen(nums[i]), str)) {
                debug_print("error when copying number = %s to big_int_str. i=%u\n", nums[i], i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting from string to number. Number=%s, i = %u\n", nums[i], i);
            }
            if (big_int_copy(a, b)) {
                debug_print("error when copying number form [a] to [b]. Number=%s, i=%u\n", nums[i], i);
            }
            big_int_cmp(a, b, &cmp_flag);
            if (cmp_flag != 0) {
                debug_print("number [a] is not equal to number [b]. [a]=%s, i=%u\n", nums[i], i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_copy, big_int_from_str & big_int_cmp test\n");
    }
    printf("\n");

    /* base_convert() */
    {
        size_t i;
        struct {
            char *from;
            unsigned int base_from;
            char *to;
            unsigned int base_to;
        } test[] = {
            {"01101111111111111111111111010110101010101010101010", 2, "2101121002022011122112111000121", 3},
            {"qwertyuoisdf90f890dsfnlksdj", 36, "7rci78i8nln0a7wtfnvr4p8tnahp", 33},
            {"123908908786876876", 10, "1b8367efcbf81cc", 16},
            {"-123908908786876876", 10, "-2014411442030143420030001", 5},
            {"12365145140122021213213213", 7, "gk6c36pn8hnea8j", 27},
            {"-0", 10, "0", 36},
        };
        big_int_str *str = NULL;

        printf("big_int_base_convert test...\n");
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("error when creating big_int_str string\n");
        }
        for (i = 0; i < sizeof(test) / sizeof(test[0]); i++) {
            if (big_int_str_copy_s(test[i].from, strlen(test[i].from), str)) {
                debug_print("error when copying string %s to big_int_str (i = %u)\n", test[i].from, i);
            }
            if (big_int_base_convert(str, str, test[i].base_from, test[i].base_to)) {
                debug_print("error when converting number %s (base %u) to base %u (i = %u)\n",
                    test[i].from, test[i].base_from, test[i].base_to, i);
            }
            if (strcmp(test[i].to, str->str)) {
                debug_print("wrong converting. str=%s, expected=%s (i = %u)\n", str->str, test[i].to, i);
            }
        }
        big_int_str_destroy(str);
        printf("end of big_int_base_convert test\n");
    }
    printf("\n");

    /* big_int_serialize() & big_int_unserialize() */
    {
        size_t i;
        int cmp_flag;
        big_int *a = NULL, *b = NULL;
        char *nums[] = {
            "12323213213873428342843287328473204732478938274238947",
            "0",
            "1",
            "-123238907435437890754325328947234239009890",
            "34324234239879080980291832137070709218370",
        };
        big_int_str *str = NULL;

        printf("big_int_serialize & big_int_unserialize tests...\n");
        a = big_int_create(1);
        b = big_int_create(1);
        if (a == NULL || b == NULL) {
            debug_print("cannot create big_int number\n");
        }
        str = big_int_str_create(1);
        if (str == NULL) {
            debug_print("cannot create big_int_str string\n");
        }
        for (i = 0; i < sizeof(nums) / sizeof(nums[0]); i++) {
            if (big_int_str_copy_s(nums[i], strlen(nums[i]), str)) {
                debug_print("error when copying string %s to big_int_str. i = %u\n", nums[i], i);
            }
            if (big_int_from_str(str, 10, a)) {
                debug_print("error when converting string %s to number [a]. i = %u\n", nums[i], i);
            }
            if (big_int_serialize(a, 1, str)) {
                debug_print("error when serializing number %s. i = %u\n", nums[i], i);
            }
            if (big_int_unserialize(str, 1, b)) {
                debug_print("error when unserializing number %s. i = %u\n", nums[i], i);
            }
            big_int_cmp(a, b, &cmp_flag);
            if (cmp_flag != 0) {
                debug_print("error when serializing/unserializing number %s. i = %u\n", nums[i], i);
            }
        }
        big_int_str_destroy(str);
        big_int_destroy(b);
        big_int_destroy(a);
        printf("end of big_int_serialize & big_int_unserialize tests\n");
    }
    printf("\n");
}
