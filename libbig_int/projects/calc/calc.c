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
    simple calculator based on BIG_INT library
*/
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> /* for rand() */
#include "basic_funcs.h"
#include "number_theory.h"
#include "bitset_funcs.h"

#if HAVE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

#define CALC_VERSION "0.0.7 " __DATE__ " " __TIME__
#define LEXEME_MAX_LEN 2048
#define LINE_MAX_LEN 16384
#define HISTORY_SIZE 100
#define FUNC_ARGS_SIZE 5
#define CALC_PROMPT "calc > "

typedef enum {
    NONE = -1,
    DONE = 256,
    DEC_NUM, HEX_NUM, BIN_NUM, OCT_NUM,
    NEXT_PRIME, IS_PRIME, JACOBI, SHL, SHR, GCD1,
    SQRT, SQR, SQRT_REM, OR1, AND1, XOR1, RANDOM,
    ABS, BIT_LEN, BIT1_CNT, HAMMING_DIST,
    SET_BIT, CLR_BIT, INV_BIT, TEST_BIT,
    SUBINT, DIV1, MOD1,
    ID,
} token_id;

/* global variables */
static struct {
    token_id token;
    char str[LEXEME_MAX_LEN + 1];
} curr_lex;

static struct {
    char buf[LINE_MAX_LEN + 1];
    char *s;
    char *s_end;
} line;

static const struct {
    token_id token;
    char *str;
} symtable[] = {
    {NEXT_PRIME, "nextprime"},
    {NEXT_PRIME, "next_prime"},
    {IS_PRIME, "isprime"},
    {IS_PRIME, "is_prime"},
    {JACOBI, "jacobi"},
    {GCD1, "gcd"},
    {SQRT, "sqrt"},
    {SQR, "sqr"},
    {SQRT_REM, "sqrt_rem"},
    {OR1, "or"},
    {AND1, "and"},
    {XOR1, "xor"},
    {RANDOM, "rand"},
    {RANDOM, "random"},
    {ABS, "abs"},
    {BIT_LEN, "bit_len"},
    {BIT1_CNT, "bit1_cnt"},
    {HAMMING_DIST, "hamming_dist"},
    {SET_BIT, "set_bit"},
    {CLR_BIT, "clr_bit"},
    {INV_BIT, "inv_bit"},
    {TEST_BIT, "test_bit"},
    {SUBINT, "subint"},
    {DIV1, "div"},
    {MOD1, "mod"},
};

/* array of history numbers */
static big_int *history[HISTORY_SIZE];
static int history_pos = 0; /* current position in the history */

static int is_mod = 0; /* flag of the modular arithmetic */
static big_int *module = NULL; /* module for modular arithmetic */

static int expr(big_int *a);
static int expr1(big_int *a);
static int expr2(big_int *a);
static int expr3(big_int *a);
static int expr4(big_int *a);
static int term(big_int *a);
static int term1(big_int *a);
static int term2(big_int *a);
static int factor(big_int *a);
static int get_func_args(char *func_name, big_int **args, int args_cnt_min, int args_cnt_max, int *args_cnt);
static int match(token_id token);
static int history_init(void);
static void history_destroy(void);
static int symtable_lookup(char *s);
static char line_next_char(void);
static void line_prev_char(void);
static int lexan(void);
static int read_line(char *prompt);
static void help(void);
static int is_zero(big_int *a);

int is_zero(big_int *a)
{
    return (a->len == 1 && a->num[0] == 0);
}

int symtable_lookup(char *s)
{
    size_t i;

    for (i = 0; i < sizeof(symtable) / sizeof(symtable[0]); i++) {
        if (!strcmp(s, symtable[i].str)) {
            return i;
        }
    }
    return -1; /* point to ID */
}

char line_next_char(void)
{
    if (line.s > line.s_end) {
        return 0;
    }
    return *(line.s++);
}

void line_prev_char(void)
{
    if (line.s <= line.buf || line.s > line.s_end) {
        return;
    }
    line.s--;
}

int lexan(void)
{
    char *s, *s_end;
    char ch, ch1;
    int i;

    while (1) {
        ch = line_next_char();
        s = curr_lex.str;
        s_end = s + LEXEME_MAX_LEN;
        if (isspace(ch)) {
            /* skip spaces */
            continue;
        }

        if (!ch) {
            /* line was read */
            *s = '\0';
            curr_lex.token = DONE;
            return 0;
        }

        if (isdigit(ch)) {
            /*
                start of number.
                Number can be:
                    0|[1-9][0-9]* - DEC_NUM
                    0[0-7]+ - OCT_NUM
                    0x[0-9a-fA-F]+ - HEX_NUM
                    0b[01]+ - BIN_NUM
            */
            if (ch != '0') {
                /* DEC_NUM */
                while (s < s_end && isdigit(ch)) {
                    *s++ = ch;
                    ch = line_next_char();
                }
                if (s == s_end) {
                    printf("too long decimal number. Max length is %d\n", LEXEME_MAX_LEN);
                    return 1;
                }
                *s = '\0';
                curr_lex.token = DEC_NUM;
                line_prev_char();
                return 0;
            }

            ch1 = line_next_char();
            switch (ch1) {
                case 'x' :
                    /* HEX_NUM */
                    ch = line_next_char();
                    while (s < s_end && isxdigit(ch)) {
                        *s++ = ch;
                        ch = line_next_char();
                    }
                    if (s == s_end) {
                        printf("too long hexadecimal number. Max length is %d\n", LEXEME_MAX_LEN);
                        return 1;
                    }
                    if (s == curr_lex.str) {
                        /* end of line */
                        printf("wrong end of hexadecimal number\n");
                        return 1;
                    }
                    *s = '\0';
                    curr_lex.token = HEX_NUM;
                    line_prev_char();
                    return 0;
                case 'b' :
                    /* BIN_NUM */
                    ch = line_next_char();
                    while (s < s_end && (ch == '0' || ch == '1')) {
                        *s++ = ch;
                        ch = line_next_char();
                    }
                    if (s == s_end) {
                        printf("too long binary number. Max length is %d\n", LEXEME_MAX_LEN);
                        return 1;
                    }
                    if (s == curr_lex.str) {
                        /* end of line */
                        printf("wrong end of binary number\n");
                        return 1;
                    }
                    *s = '\0';
                    curr_lex.token = BIN_NUM;
                    line_prev_char();
                    return 0;
                default :
                    /* OCT_NUM */
                    *s++ = ch;
                    ch = ch1;
                    while (s < s_end && ch - '0' >= 0 && ch - '0' < 8) {
                        *s++ = ch;
                        ch = line_next_char();
                    }
                    if (s == s_end) {
                        printf("too long octal number. Max length is %d\n", LEXEME_MAX_LEN);
                        return 1;
                    }
                    *s = '\0';
                    curr_lex.token = OCT_NUM;
                    line_prev_char();
                    return 0;
            }
        } /* if isxdigit(ch) */

        if (isalpha(ch) || ch == '_') {
            /* ID */
            while (s < s_end && (isalnum(ch) || ch == '_')) {
                *s++ = (char) tolower(ch);
                ch = line_next_char();
            }
            if (s == s_end) {
                printf("too long id. Max length is %d\n", LEXEME_MAX_LEN);
                return 1;
            }
            *s = '\0';
            i = symtable_lookup(curr_lex.str);
            curr_lex.token = (i < 0) ? ID : symtable[i].token;
            line_prev_char();
            return 0;
        }

        if (ch == '>') {
            if (line_next_char() == '>') {
                *s++ = ch;
                *s++ = ch;
                *s = '\0';
                curr_lex.token = SHR;
                return 0;
            }
            line_prev_char();
        }

        if (ch == '<') {
            if (line_next_char() == '<') {
                *s++ = ch;
                *s++ = ch;
                *s = '\0';
                curr_lex.token = SHL;
                return 0;
            }
            line_prev_char();
        }

        /* token is [ch] */
        *s++ = ch;
        *s = '\0';
        curr_lex.token = (token_id) ch;
        return 0;
    }
    return 0;
}

int read_line(char *prompt)
{
#if HAVE_READLINE
    char *str;

    str = readline(prompt);
    if (str == NULL) {
        /* end of file */
        return 1;
    }
    strncpy(line.buf, str, LINE_MAX_LEN);
    line.buf[LINE_MAX_LEN] = '\0';
    free(str);
#else
    printf("%s", prompt);
    if (fgets(line.buf, LINE_MAX_LEN + 1, stdin) == NULL) {
        /* end of file */
        return 1;
    }
#endif
    line.s = line.buf;
    line.s_end = line.s + strlen(line.s);

    return 0;
}

void help(void)
{
    puts("Help for BIG_INT calculator:");
    puts("Numbers can have the following format:");
    puts("[1-9][0-9]*     decimal numbers");
    puts("0[0-7]+         octal numbers");
    puts("0x[0-9a-fA-F]+  hex numbers");
    puts("0b[01]+         binary numbers");
    puts("");
    puts("The following operators and functions are implemented:");
    puts("    binary operators: +, -, *, /, %, ^, >>, <<, &, |, div, mod, and, or, xor");
    puts("    unary operators: -, +, !,");
    puts("    functions: is_prime(n), next_prime(n),");
    puts("        gcd(n1, n2), jacobi(n1, n2), hamming_dist(n1, n2),");
    puts("        sqrt(n), sqrt_rem(n), sqr(n), rand(bits_cnt), abs(n),");
    puts("        set_bit(n, bit_pos), clr_bit(n, bit_pos), inv_bit(n, bit_pos),");
    puts("        test_bit(n, bit_pos), subint(n, bit_start, bit_len [, is_invert])");
    puts("");
    puts("You can access to numbers from history by $n,");
    printf("where n can be from 1 to %d,\n", HISTORY_SIZE);
    puts("Also you can access previous result by $$");
    puts("");
    puts("One line can contain many expressions, delimited by comma:");
    puts("[expr_1], [expr_2], ... , [expr_n]");
    puts("");
    puts("The following commands is avaible:");
    puts("\\? and \\h - show this help");
    puts("\\b [base] - chage output base. It can be from 2 to 36 inclusive");
    puts("\\m [expr] - switch to the modular arithmetic with module [expr]");
    puts("\\i - switch to the integer arithmetic");
    puts("\\s - view current settings");
    puts("\\q - quit the calculator");
    puts("");
}

int match(token_id token)
{
    if (token == curr_lex.token) {
        if (lexan()) {
            /* error */
            return 0;
        }
        return 1;
    }
    /* not match */
    return 0;
}

int get_func_args(char *func_name, big_int **args,
                  int args_cnt_min, int args_cnt_max, int *args_cnt)
{
    int result = 0;

    assert(args_cnt_max <= FUNC_ARGS_SIZE);

    *args_cnt = 0;
    if (!match('(')) {
        printf("expected '(' after function name [%s]\n", func_name);
        result = 1;
        goto done;
    }

    if (curr_lex.token == ')') {
        /* empty arguments list */
        match(')');
        goto done;
    }

    /* arguments list is not empty */
    while (1) {
        /* check parameters count, passed to the function */
        if (*args_cnt >= args_cnt_max) {
            printf("function [%s] cannot have more than %d arguments\n", func_name, args_cnt_max);
            result = 2;
            goto done;
        }

        /* init func_args[func_args_cnt] */
        args[*args_cnt] = big_int_create(1);
        if (args[*args_cnt] == NULL) {
            printf("error when creating args[%d]\n", *args_cnt);
            result = 3;
            goto done;
        }

        /* parse next paramter */
        if (expr(args[(*args_cnt)++])) {
            result = 4;
            goto done;
        }

        if (curr_lex.token == ')') {
            /* end of arguments list */
            match(')');
            goto done;
        }

        /* go to the next argument */
        if (!match(',')) {
            printf("expected ',' or ')' after argument number %d in the function [%s]\n",* args_cnt, func_name);
            result = 5;
            goto done;
        }
    }

done:
    if (!result && *args_cnt < args_cnt_min) {
        printf("function [%s] cannot have less than %d arguments\n", func_name, args_cnt_min);
        result = 7;
        goto done;
    }

    return result;
}

int factor(big_int *a)
{
    int result = 0;
    int is_prime, n;
    big_int_str str;
    big_int *func_args[FUNC_ARGS_SIZE];
    int func_args_cnt = 0;
    int i;

    str.str = curr_lex.str;
    str.len = str.len_allocated = strlen(str.str);
    switch ((int) curr_lex.token) {
        case '(' :
            match('(');
            if (expr(a)) {
                result = 2;
                goto done;
            }
            if (!match(')')) {
                printf("wrong syntax. Expected ')', found %s\n", str.str);
                result = 3;
                goto done;
            }
            break;
        case DEC_NUM :
            if (big_int_from_str(&str, 10, a)) {
                printf("cannot convert string [%s] to decimal number\n", str.str);
                result = 4;
                goto done;
            }
            match(DEC_NUM);
            break;
        case HEX_NUM :
            if (big_int_from_str(&str, 16, a)) {
                printf("cannot convert string [%s] to hexadecimal number\n", str.str);
                result = 5;
                goto done;
            }
            match(HEX_NUM);
            break;
        case OCT_NUM :
            if (big_int_from_str(&str, 8, a)) {
                printf("cannot convert string [%s] to octal number\n", str.str);
                result = 6;
                goto done;
            }
            match(OCT_NUM);
            break;
        case BIN_NUM :
            if (big_int_from_str(&str, 2, a)) {
                printf("cannot convert string [%s] to binary number\n", str.str);
                result = 7;
                goto done;
            }
            match(BIN_NUM);
            break;
        case NEXT_PRIME :
            match(NEXT_PRIME);
            if (get_func_args("next_prime", func_args, 1, 1, &func_args_cnt)) {
                result = 8;
                goto done;
            }
            if (big_int_next_prime(func_args[0], a)) {
                printf("error in big_int_next_prime()\n");
                result = 9;
                goto done;
            }
            break;
        case IS_PRIME :
            match(IS_PRIME);
            if (get_func_args("is_prime", func_args, 1, 1, &func_args_cnt)) {
                result = 10;
                goto done;
            }
            if (big_int_is_prime(func_args[0], 100, 1, &is_prime)) {
                printf("error in big_int_is_prime()\n");
                result = 11;
                goto done;
            }
            if (big_int_from_int(is_prime, a)) {
                printf("error in big_int_from_int()\n");
                result = 12;
                goto done;
            }
            break;
        case GCD1 :
            match(GCD1);
            if (get_func_args("gcd", func_args, 2, 2, &func_args_cnt)) {
                result = 13;
                goto done;
            }
            if (big_int_gcd(func_args[0], func_args[1], a)) {
                printf("error in big_int_gcd()\n");
                result = 14;
                goto done;
            }
            break;
        case SQRT :
            match(SQRT);
            if (get_func_args("sqrt", func_args, 1, 1, &func_args_cnt)) {
                result = 15;
                goto done;
            }
            if (func_args[0]->sign == MINUS) {
                printf("cannot calculate square root from negative number\n");
                result = 16;
                goto done;
            }
            if (big_int_sqrt(func_args[0], a)) {
                printf("error in big_int_sqrt()\n");
                result = 17;
                goto done;
            }
            break;
        case SQR :
            match(SQR);
            if (get_func_args("sqr", func_args, 1, 1, &func_args_cnt)) {
                result = 18;
                goto done;
            }
            if (big_int_sqr(func_args[0], a)) {
                printf("error in big_int_sqr()\n");
                result = 19;
                goto done;
            }
            break;
        case SQRT_REM :
            match(SQRT_REM);
            if (get_func_args("sqrt_rem", func_args, 1, 1, &func_args_cnt)) {
                result = 19;
                goto done;
            }
            if (func_args[0]->sign == MINUS) {
                printf("cannot calculate square root from negative number\n");
                result = 16;
                goto done;
            }
            if (big_int_sqrt_rem(func_args[0], a)) {
                printf("error in big_int_sqrt_rem()\n");
                result = 20;
                goto done;
            }
            break;
        case RANDOM :
            match(RANDOM);
            if (get_func_args("random", func_args, 1, 1, &func_args_cnt)) {
                result = 21;
                goto done;
            }
            if (big_int_to_int(func_args[0], &n)) {
                printf("error in big_int_to_int()\n");
                result = 22;
                goto done;
            }
            if (n < 0) {
                printf("argument of random() must be positive\n");
                result = 23;
                goto done;
            }
            if (big_int_rand(rand, (size_t) n, a)) {
                printf("error in big_int_rand()\n");
                result = 24;
                goto done;
            }
            break;
        case ABS :
            match(ABS);
            if (get_func_args("abs", func_args, 1, 1, &func_args_cnt)) {
                result = 25;
                goto done;
            }
            if (big_int_abs(func_args[0], a)) {
                printf("error in big_int_abs()\n");
                result = 26;
                goto done;
            }
            break;
        case BIT_LEN :
            match(BIT_LEN);
            if (get_func_args("bit_len", func_args, 1, 1, &func_args_cnt)) {
                result = 27;
                goto done;
            }
            big_int_bit_length(func_args[0], (unsigned int *) &n);
            if (big_int_from_int(n, a)) {
                printf("error in big_int_from_int()\n");
                result = 28;
                goto done;
            }
            break;
        case BIT1_CNT :
            match(BIT1_CNT);
            if (get_func_args("bit1_cnt", func_args, 1, 1, &func_args_cnt)) {
                result = 29;
                goto done;
            }
            big_int_bit1_cnt(func_args[0], (unsigned int *) &n);
            if (big_int_from_int(n, a)) {
                printf("error in big_int_from_int()\n");
                result = 30;
                goto done;
            }
            break;
        case HAMMING_DIST :
            match(HAMMING_DIST);
            if (get_func_args("hamming_dist", func_args, 2, 2, &func_args_cnt)) {
                result = 31;
                goto done;
            }
            if (big_int_hamming_distance(func_args[0], func_args[1], (unsigned int *) &n)) {
                printf("error in big_int_hamming_distance()\n");
                result = 32;
                goto done;
            }
            if (big_int_from_int(n, a)) {
                printf("error in big_int_from_int()\n");
                result = 33;
                goto done;
            }
            break;
        case SET_BIT :
            match(SET_BIT);
            if (get_func_args("set_bit", func_args, 2, 2, &func_args_cnt)) {
                result = 34;
                goto done;
            }
            if (big_int_to_int(func_args[1], &n)) {
                printf("error in big_int_to_int()\n");
                result = 35;
                goto done;
            }
            if (big_int_set_bit(func_args[0], (size_t) n, a)) {
                printf("error in big_int_set_bit()\n");
                result = 36;
                goto done;
            }
            break;
        case CLR_BIT :
            match(CLR_BIT);
            if (get_func_args("clr_bit", func_args, 2, 2, &func_args_cnt)) {
                result = 37;
                goto done;
            }
            if (big_int_to_int(func_args[1], &n)) {
                printf("error in big_int_to_int()\n");
                result = 38;
                goto done;
            }
            if (big_int_clr_bit(func_args[0], (size_t) n, a)) {
                printf("error in big_int_clr_bit()\n");
                result = 39;
                goto done;
            }
            break;
        case INV_BIT :
            match(INV_BIT);
            if (get_func_args("inv_bit", func_args, 2, 2, &func_args_cnt)) {
                result = 40;
                goto done;
            }
            if (big_int_to_int(func_args[1], &n)) {
                printf("error in big_int_to_int()\n");
                result = 41;
                goto done;
            }
            if (big_int_inv_bit(func_args[0], (size_t) n, a)) {
                printf("error in big_int_inv_bit()\n");
                result = 42;
                goto done;
            }
            break;
        case TEST_BIT :
            match(TEST_BIT);
            if (get_func_args("test_bit", func_args, 2, 2, &func_args_cnt)) {
                result = 43;
                goto done;
            }
            if (big_int_to_int(func_args[1], &n)) {
                printf("error in big_int_to_int()\n");
                result = 44;
                goto done;
            }
            if (big_int_test_bit(func_args[0], (size_t) n, &n)) {
                printf("error in big_int_test_bit()\n");
                result = 45;
                goto done;
            }
            if (big_int_from_int(n, a)) {
                printf("error in big_int_from_int()\n");
                result = 46;
                goto done;
            }
            break;
        case JACOBI :
            match(JACOBI);
            if (get_func_args("jacobi", func_args, 2, 2, &func_args_cnt)) {
                result = 47;
                goto done;
            }
            if ((func_args[1]->num[0] & 1) == 0) {
                printf("second argument of jacobi() must be odd\n");
                result = 48;
                goto done;
            }
            if (big_int_jacobi(func_args[0], func_args[1], &n)) {
                printf("error in big_int_jacobi()\n");
                result = 49;
                goto done;
            }
            if (big_int_from_int(n, a)) {
                printf("error in big_int_from_int()\n");
                result = 50;
                goto done;
            }
            break;
        case SUBINT :
            match(SUBINT);
            if (get_func_args("subint", func_args, 3, 4, &func_args_cnt)) {
                result = 53;
                goto done;
            }
            {
                size_t bit_start, bit_len;
                int is_invert;
                if (big_int_to_int(func_args[1], (int *) &bit_start)) {
                    printf("error in big_int_to_int()\n");
                    result = 54;
                    goto done;
                }
                if (big_int_to_int(func_args[2], (int *) &bit_len)) {
                    printf("error in big_int_to_int()\n");
                    result = 55;
                    goto done;
                }
                if (func_args_cnt == 4) {
                    if (big_int_to_int(func_args[3], &is_invert)) {
                        printf("error in big_int_to_int()\n");
                        result = 56;
                        goto done;
                    }
                } else {
                    is_invert = 0;
                }
                if (big_int_subint(func_args[0], bit_start, bit_len, is_invert, a)) {
                    printf("error in big_int_subint()\n");
                    result = 57;
                    goto done;
                }
            }
            break;
        case DONE :
            printf("unexpected end of line.\n");
            result = 58;
            goto done;
            break;
        case '$' :
            match('$');
            if (curr_lex.token != DEC_NUM && curr_lex.token != '$') {
                printf("expected decimal number greater than zero or '$' after symbol '$'\n");
                result = 59;
                goto done;
            }
            if (curr_lex.token == '$') {
                /* return last number from history */
                match('$');
                n = history_pos;
                if (n == 0) {
                    n = HISTORY_SIZE;
                }
                if (big_int_copy(history[n - 1], a)) {
                    printf("error when copying variable $%d to [a]\n", n);
                    result = 60;
                    goto done;
                }
                goto done;
            }
            str.str = curr_lex.str;
            str.len = str.len_allocated = strlen(str.str);
            if (big_int_from_str(&str, 10, a)) {
                printf("error whe converting string [%s] to number\n", str.str);
                result = 61;
                goto done;
            }
            if (big_int_to_int(a, &n)) {
                printf("error when converting number [a] to int [n]\n");
                result = 62;
                goto done;
            }
            if (!n || n > HISTORY_SIZE) {
                printf("there is no variable $%d\n", n);
                result = 63;
                goto done;
            }
            if (big_int_copy(history[n - 1], a)) {
                printf("error when copying variable $%d to [a]\n", n);
                result = 64;
                goto done;
            }
            match(DEC_NUM);
            break;
        default :
            printf("syntax error. Unexpected lexeme [%s]. Token id = %d\n", str.str, curr_lex.token);
            result = 65;
            goto done;
    }

done:
    /* free func_args array */
    for (i = 0; i < func_args_cnt; i++) {
        big_int_destroy(func_args[i]);
    }
    return result;
}

int term2(big_int *a)
{
    int n;
    int result = 0;
    int sign = 0;

    while (1) {
        switch ((int) curr_lex.token) {
            case '+' :
                /* Unary plus. Just skip it. */
                match('+');
                continue;
            case '-' :
                /* Unary minus. Invert following operand */
                match('-');
                sign ^= 1;
                continue;
            default :
                /* just call factor() */
                if (factor(a)) {
                    result = 1;
                    goto done;
                }
                if (sign) {
                    /* invert the sign of number */
                    if (big_int_neg(a, a)) {
                        printf("error when inverting the sign of number [a]\n");
                        result = 2;
                        goto done;
                    }
                }
                if (curr_lex.token == '!') {
                    /* factorial */
                    match('!');
                    if (is_mod) {
                        /* modular arithmetic */
                        if (big_int_factmod(a, module, a)) {
                            printf("error in big_int_factmod()\n");
                            result = 3;
                            goto done;
                        }
                    } else {
                        /* ordinary arithmetic */
                        if (big_int_to_int(a, &n)) {
                            printf("error when converting number [a] to int [n]\n");
                            result = 4;
                            goto done;
                        }
                        if (big_int_fact(n, a)) {
                            printf("error in big_int_fact(%d)\n", n);
                            result = 5;
                            goto done;
                        }
                    }
                }
                goto done;
        }
    }

done:
    return result;
}

int term1(big_int *a)
{
    big_int *b = NULL;
    int n;
    int result = 0;

    b = big_int_create(1);
    if (b == NULL) {
        printf("error when creating [b]\n");
        result = 1;
        goto done;
    }

    if (term2(a)) {
        result = 2;
        goto done;
    }

    while (1) {
        switch ((int) curr_lex.token) {
            case '^' :
                match('^');
                if (term2(b)) {
                    result = 3;
                    goto done;
                }
                if (is_mod) {
                    if (big_int_powmod(a, b, module, a)) {
                        printf("error during big_int_powmod()\n");
                        result = 4;
                        goto done;
                    }
                } else {
                    if (big_int_to_int(b, &n)) {
                        printf("error when converting number [b] to int [n]\n");
                        result = 5;
                        goto done;
                    }
                    if (big_int_pow(a, n, a)) {
                        printf("error during big_int_pow()\n");
                        result = 6;
                        goto done;
                    }
                }
                continue;
            default :
                goto done;
        }
    }

done:
    big_int_destroy(b);
    return result;
}

int term(big_int *a)
{
    big_int *b = NULL;
    int result = 0;

    b = big_int_create(1);
    if (b == NULL) {
        printf("error when creating [b]\n");
        result = 1;
        goto done;
    }

    if (term1(a)) {
        result = 2;
        goto done;
    }

    while (1) {
        switch ((int) curr_lex.token) {
            case '*' :
                match('*');
                if (term1(b)) {
                    result = 3;
                    goto done;
                }
                if (is_mod) {
                    if (big_int_mulmod(a, b, module, a)) {
                        printf("error in big_int_mulmod()\n");
                        result = 4;
                        goto done;
                    }
                } else {
                    if (big_int_mul(a, b, a)) {
                        printf("error in big_int_mul()\n");
                        result = 5;
                        goto done;
                    }
                }
                continue;
            case '/' : case DIV1 :
                match(curr_lex.token);
                if (term1(b)) {
                    result = 6;
                    goto done;
                }
                if (is_zero(b)) {
                    printf("division by zero\n");
                    result = 7;
                    goto done;
                }
                if (is_mod) {
                    switch (big_int_divmod(a, b, module, a)) {
                    case 0: break;
                    case 2:
                        printf("GCD(b, modulus) != 1\n");
                        result = 8;
                        goto done;
                    default :
                        printf("error in big_int_divmod()\n");
                        result = 8;
                        goto done;
                    }
                } else {
                    if (big_int_div(a, b, a)) {
                        printf("error in big_int_div()\n");
                        result = 9;
                        goto done;
                    }
                }
                continue;
            case '%' : case MOD1 :
                match(curr_lex.token);
                if (term1(b)) {
                    result = 10;
                    goto done;
                }
                if (is_zero(b)) {
                    printf("division by zero\n");
                    result = 11;
                    goto done;
                }
                if (big_int_mod(a, b, a)) {
                    printf("error in big_int_mod()\n");
                    result = 12;
                    goto done;
                }
                continue;
            default :
                goto done;
        }
    }

done:
    big_int_destroy(b);
    return result;
}

int expr4(big_int *a)
{
    big_int *b = NULL;
    int result = 0;

    b = big_int_create(1);
    if (b == NULL) {
        printf("error when creating [b]\n");
        result = 1;
        goto done;
    }

    if (term(a)) {
        result = 2;
        goto done;
    }

    while (1) {
        switch ((int) curr_lex.token) {
            case '+' :
                match('+');
                if (term(b)) {
                    result = 3;
                    goto done;
                }
                if (is_mod) {
                    if (big_int_addmod(a, b, module, a)) {
                        printf("error in big_int_addmod()\n");
                        result = 4;
                        goto done;
                    }
                } else {
                    if (big_int_add(a, b, a)) {
                        printf("error in big_int_add()\n");
                        result = 5;
                        goto done;
                    }
                }
                continue;
            case '-' :
                match('-');
                if (term(b)) {
                    result = 6;
                    goto done;
                }
                if (is_mod) {
                    if (big_int_submod(a, b, module, a)) {
                        printf("error in big_int_submod()\n");
                        result = 7;
                        goto done;
                    }
                } else {
                    if (big_int_sub(a, b, a)) {
                        printf("error in big_int_sub()\n");
                        result = 8;
                        goto done;
                    }
                }
                continue;
            default :
                goto done;
        }
    }

done:
    big_int_destroy(b);
    return result;
}

int expr3(big_int *a)
{
    big_int *b = NULL;
    int result = 0;
    int n;

    b = big_int_create(1);
    if (b == NULL) {
        printf("error when creating [b]\n");
        result = 1;
        goto done;
    }

    if (expr4(a)) {
        result = 2;
        goto done;
    }

    while (1) {
        switch (curr_lex.token) {
            case SHL :
                match(SHL);
                if (expr4(b)) {
                    result = 3;
                    goto done;
                }
                if (big_int_to_int(b, &n)) {
                    printf("error when converting number [b] to int [n]\n");
                    result = 4;
                    goto done;
                }
                if (big_int_lshift(a, n, a)) {
                    printf("error in big_int_lshift(a, %d)\n", n);
                    result = 5;
                    goto done;
                }
                if (is_mod) {
                    if (big_int_absmod(a, module, a)) {
                        printf("error in big_int_absmod()\n");
                        result = 6;
                        goto done;
                    }
                }
                continue;
            case SHR :
                match(SHR);
                if (expr4(b)) {
                    result = 7;
                    goto done;
                }
                if (big_int_to_int(b, &n)) {
                    printf("error when converting number [b] to int [n]\n");
                    result = 8;
                    goto done;
                }
                if (big_int_rshift(a, n, a)) {
                    printf("error in big_int_rshift(a, %d)\n", n);
                    result = 9;
                    goto done;
                }
                if (is_mod) {
                    if (big_int_absmod(a, module, a)) {
                        printf("error in big_int_absmod()\n");
                        result = 10;
                        goto done;
                    }
                }
                continue;
            default :
                goto done;
        }
    }

done:
    big_int_destroy(b);
    return result;
}

int expr2(big_int *a)
{
    big_int *b = NULL;
    int result = 0;

    b = big_int_create(1);
    if (b == NULL) {
        printf("error when creating [b]\n");
        result = 1;
        goto done;
    }

    if (expr3(a)) {
        result = 2;
        goto done;
    }

    while (1) {
        switch ((int) curr_lex.token) {
            case AND1 : case '&' :
                match(curr_lex.token);
                if (expr3(b)) {
                    result = 3;
                    goto done;
                }
                if (big_int_and(a, b, 0, a)) {
                    printf("error in big_int_and()\n");
                    result = 4;
                    goto done;
                }
                continue;
            default :
                goto done;
        }
    }

done:
    big_int_destroy(b);
    return result;
}

int expr1(big_int *a)
{
    big_int *b = NULL;
    int result = 0;

    b = big_int_create(1);
    if (b == NULL) {
        printf("error when creating [b]\n");
        result = 1;
        goto done;
    }

    if (expr2(a)) {
        result = 2;
        goto done;
    }

    while (1) {
        switch (curr_lex.token) {
            case XOR1 :
                match(XOR1);
                if (expr2(b)) {
                    result = 3;
                    goto done;
                }
                if (big_int_xor(a, b, 0, a)) {
                    printf("error in big_int_xor()\n");
                    result = 4;
                    goto done;
                }
                continue;
            default :
                goto done;
        }
    }

done:
    big_int_destroy(b);
    return result;
}

int expr(big_int *a)
{
    big_int *b = NULL;
    int result = 0;

    b = big_int_create(1);
    if (b == NULL) {
        printf("error when creating [b]\n");
        result = 1;
        goto done;
    }

    if (expr1(a)) {
        result = 2;
        goto done;
    }

    while (1) {
        switch ((int) curr_lex.token) {
            case OR1 : case '|' :
                match(curr_lex.token);
                if (expr1(b)) {
                    result = 3;
                    goto done;
                }
                if (big_int_or(a, b, 0, a)) {
                    printf("error in big_int_or()\n");
                    result = 4;
                    goto done;
                }
                continue;
            default :
                goto done;
        }
    }

done:
    big_int_destroy(b);
    return result;
}

int parse(void)
{
    big_int *a = NULL;
    big_int_str *str = NULL;
    int result = 0;
    unsigned int out_base = 10;

    a = big_int_create(1);
    str = big_int_str_create(1);
    if (a == NULL || str == NULL) {
        printf("error when creating [a] or [str]\n");
        result = 1;
        goto done;
    }

    while (1) {
        if (read_line(CALC_PROMPT)) {
            break;
        }
        if (line_next_char() == '\\') {
            /* command */
            switch (line_next_char()) {
                case 'q' : /* quit */
                    printf("quit\n");
                    goto done;
                case '?' : case 'h' : /* help */
                    help();
                    continue;
                case 'b' : /* change output base. Default is 10 */
                    if (sscanf(line.s, "%u", &out_base) != 1) {
                        printf("cannot recogize new base\n");
                    }
                    if (out_base < 2 || out_base > 36) {
                        printf("wrong value of base. Acceptible value is [2 - 36]\n");
                        out_base = 10;
                    } else {
                        printf("new base is %u\n", out_base);
                    }
                    continue;
                case 'm' : /* switch to modular arithmetic */
                    /* calculate module */
                    if (lexan()) {
                        continue;
                    }
                    is_mod = 0; /* disable module arithmetic */
                    if (expr(a)) {
                        /* error when calculating module */
                        continue;
                    }
                    if (big_int_copy(a, module)) {
                        printf("error when copying number [a] to [module]\n");
                        continue;
                    }
                    module->sign = PLUS;
                    if (module->len == 1 && module->num[0] < 2) {
                        printf("module must be greater than 1\n");
                        continue;
                    }
                    is_mod = 1; /* enable module arithmetic */
                    if (big_int_to_str(module, out_base, str)) {
                        printf("error during converting number to string\n");
                        continue;
                    }
                    printf("Switching to modular arithmetic. Module is %s\n", str->str);
                    continue;
                case 'i' : /* switch to integer arithmetic */
                    is_mod = 0;
                    printf("Switching to integer arithmetic\n");
                    continue;
                case 's' : /* show current settings */
                    puts("Current settings:");
                    printf("Base: %u\n", out_base);
                    printf("Mode: %s arithmetic\n", is_mod ? "modular (\\m)" : "integer (\\i)");
                    continue;
                default :
                    /* unknown command */
                    printf("unknown command\n");
                    continue;
            }
        }
        line_prev_char();
        /* parse line */
        if (lexan() || curr_lex.token == DONE) {
            continue;
        }
        while (!expr(a)) {
            if (curr_lex.token != ',' && curr_lex.token != DONE) {
                printf("wrong lexeme [%s]. Token id=%d. Expected ',' or \\n\n",
                    curr_lex.str, curr_lex.token);
                break;
            }
            match(','); /* go to the next expression */
            if (is_mod) {
                if (big_int_absmod(a, module, a)) {
                    printf("error in big_int_absmod()\n");
                    continue;
                }
            }
            if (big_int_to_str(a, out_base, str)) {
                printf("error during converting number to string\n");
                continue;
            }
            /* save result to the history */
            if (big_int_copy(a, history[history_pos])) {
                printf("error when copying number [a] to the history\n");
                continue;
            }
            /* print result to the display */
            history_pos++;
            printf("$%d = %s\n", history_pos, str->str);
            if (history_pos >= HISTORY_SIZE) {
                history_pos = 0;
            }
            if (curr_lex.token == DONE) {
                break;
            }
        }
    }

done:
    big_int_str_destroy(str);
    big_int_destroy(a);
    return result;
}

int history_init(void)
{
    int i;

    for (i = 0; i < HISTORY_SIZE; i++) {
        history[i] = big_int_create(1);
        if (history[i] == NULL) {
            printf("error when creating history[%d]\n", i);
            return 1;
        }
    }
    history_pos = 0;
    return 0;
}

void history_destroy(void)
{
    int i;

    for (i = 0; i < HISTORY_SIZE; i++) {
        big_int_destroy(history[i]);
    }
}

int main(void)
{
    int result = 0;

    printf("BIG_INT calculator version %s\n", CALC_VERSION);
    printf("Created by valyala (valyala@gmail.com) http://valyala.narod.ru\n");
    printf("Size of word is %zu bits\n", BIG_INT_WORD_BITS_CNT);
    printf("BIB_INT library version: %s, build_date: %s\n", big_int_version(), big_int_build_date());
    printf("type \\h or \\? for help\n");

    if (history_init()) {
        /* error when initializing history[] array */
        result = 1;
        goto done;
    }

    /* init the [module] */
    module = big_int_create(1);
    if (module == NULL) {
        printf("cannot create [module]\n");
        result = 2;
        goto done;
    }

    /* start parsing */
    parse();

done:
    /* free allocated memory */
    big_int_destroy(module);
    history_destroy();

    return result;
}
