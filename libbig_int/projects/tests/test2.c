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
/*
    Performance stress-test.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "big_int_full.h" /* include all BIG_INT headers */

/* global variables */
int is_print_nums = 0;

/***********************************************/
void print_num(const char *tpl, const big_int *num)
{
    big_int_str *s = NULL;

    if (!is_print_nums) {
        return;
    }

    s = big_int_str_create(1);
    if (s == NULL) {
        printf("error when creating [s]\n");
        return;
    }
    if (big_int_to_str(num, 10, s)) {
        printf("error when converting number [num] to string [s]\n");
        return;
    }
    printf(tpl, s->str);
    big_int_str_destroy(s);
}

/**
    generates [answer] such as:
        1) answer < a
        2) GCD(a, answer) == 1

    Returns:
        0 - no errors
        other - internal error
*/
int generate_inter_prime(const big_int *a, big_int *answer)
{
    unsigned int n_bits;
    int result = 0;
    big_int *tmp = NULL;
    big_int *a_copy = NULL;

    /* calculate bit length of [a] */
    big_int_bit_length(a, &n_bits);
    --n_bits; /* in order to [answer] < [a] (see below cycle) */

    /* initialize temporary number [tmp] */
    tmp = big_int_create(1);
    if (tmp == NULL) {
        result = 1;
        goto error;
    }

    /* create copy of [a], if it points to the same number as [answer] */
    if (a == answer) {
        a_copy = big_int_dup(a);
    } else {
        a_copy = (big_int *) a;
    }

    do {
        /*
            generate random number [answer] < [a]
            DO NOT USE rand() function as random generator
            in real-world cryptography!
        */
        if (big_int_rand(rand, n_bits, answer)) {
            result = 2;
            goto error;
        }
        /* calculate GCD(a, answer) */
        if (big_int_gcd(a_copy, answer, tmp)) {
            result = 3;
            goto error;
        }
    } while (tmp->len != 1 || tmp->num[0] != 1);
    /* [tmp] == 1, so GCD(a, answer) == 1 */

error:
    /* free allocated memory */
    if (a_copy != a) {
        big_int_destroy(a_copy);
    }
    big_int_destroy(tmp);
    return result;
}

int main(int argc, char *argv[])
{
    big_int *a = NULL, *b = NULL, *c = NULL;
    int result = 0;

    /* determine, print numbers or not */
    if (argc == 1) {
        printf("Usage:\n");
        printf("%s --print_nums=yes\n", argv[0]);
        printf("%s --print_nums=no\n", argv[0]);
        goto final;
    } else if (argc == 2) {
        if (!strcmp(argv[1], "--print_nums=yes")) {
            is_print_nums = 1;
        } else if (!strcmp(argv[1], "--print_nums=no")) {
            is_print_nums = 0;
        } else {
            printf("wrong parameter [%s]. Expected [--print_nums=yes] or [--print_nums=no]\n", argv[1]);
            goto final;
        }
    } else {
        printf("wrong parameters count.\n");
        printf("Expected only one parameter [--print_nums=yes] or [--print_nums=no]\n");
        goto final;
    }

    printf("Start of performance stress-test...\n");
    printf("Size of word is %zu bits\n\n", BIG_INT_WORD_BITS_CNT);
    /* initialize of [a], [b] & [c] */
    a = big_int_create(1);
    b = big_int_create(1);
    c = big_int_create(1);
    if (a == NULL || b == NULL || c == NULL) {
        printf("error when creating [a], [b] or [c]\n");
        result = 1;
        goto final;
    }

    /* calculate 3000! */
    printf("Start of calculating 3000!...");
    if (big_int_fact(3000, a)) {
        printf("error during calculating 3000!\n");
        result = 2;
        goto final;
    }
    printf("end\n");
    print_num("3000!=%s\n", a);
    printf("\n");

    /* calculate inter prime to 3000! */
    printf("Start of calculating inter prime to 3000!...");
    printf("end\n");
    if (generate_inter_prime(a, b)) {
        printf("error during calculating inter prime\n");
        result = 3;
        goto final;
    }
    print_num("inter_prime(3000!)=%s\n", b);
    printf("\n");

    /* calculating 17^1000 */
    printf("Start of calculating 17^3000...");
    if (big_int_from_int(17, a)) {
        printf("error in big_int_from_int\n");
        result = 4;
        goto final;
    }
    if (big_int_pow(a, 3000, a)) {
        printf("error during calculating 17^3000\n");
        result = 5;
        goto final;
    }
    printf("end\n");
    print_num("17^3000=%s\n", a);
    printf("\n");

    /*
        trying to find nextprime(2^1024).

        This test estimates the speed of generating 2048-bit
        RSA key n = p * q, where p and q - different prime numbers
    */
    printf("Start of finding nextprime(2^1024)...");
    if (big_int_from_int(2, a)) {
        printf("error in big_int_from_int\n");
        result = 6;
        goto final;
    }
    if (big_int_pow(a, 1024, a)) {
        printf("error during calculating 2^1024\n");
        result = 7;
        goto final;
    }
    if (big_int_next_prime(a, a)) {
        printf("error during finding nextprime(2^1024)\n");
        result = 8;
        goto final;
    }
    printf("end\n");
    print_num("nextprime(2^1024)=%s\n", a);
    printf("\n");

    /*
        calculate 65537^p = 1 (mod p),
        where p = 2^2048
        Result is proven by Euler theoreme:
            Phi(p) = 2^2047, so 65537^p = (65537^Phi(p))^2 = 1^2 = 1 (mod p)

        This test estimates the speed of encrypting one block by 2048-bit
        RSA key.
    */
    printf("Start of calculating 65537^p (mod p), where p = 2^2048...");
    if (big_int_from_int(2, a)) {
        printf("error in big_int_from_int\n");
        result = 9;
        goto final;
    }
    if (big_int_pow(a, 2048, a)) {
        printf("error during calculating 2^2048\n");
        result = 10;
        goto final;
    }
    if (big_int_from_int(65537, b)) {
        printf("error in big_int_from_int\n");
        result = 11;
        goto final;
    }
    if (big_int_powmod(b, a, a, c)) {
        printf("error during calculating 65537^p (mod p)\n");
        result = 12;
        goto final;
    }
    printf("end\n");
    print_num("65537^p = %s (mod p)\n", c);
    print_num("p = 2^2048 = %s\n", a);
    printf("\n");

    printf("End of performance stress-test\n");

final:
    /* free allocated memory */
    big_int_destroy(c);
    big_int_destroy(b);
    big_int_destroy(a);
    return result;
}
