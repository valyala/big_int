<?php
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

require_once(dirname(__FILE__) . '/std_header.php');

echo '<h1>BIG_INT test and example of usage</h1>' . "\n";

// assign a big number to $a
$a = bi_from_str('12345678901234567890');
// wiew, what returns var_dump($a) (it is NOT a simple integer, but BIG_INT resource)
echo 'var_dump($a) = ' , "<br/>\n";
var_dump($a);
echo "<br/>\n";
// now show decimal value of $a
echo '$a = [', bi_to_str($a), "]<br/>\n";

/*
    second argument of bi_from_str() and bi_to_str() - is the base
    of string representation of number. It can be from 2 to 36 inclusive
*/
$str = 'abcdef012789';
echo 'hex value of $str = [', $str, "]<br/>\n";
$b = bi_from_str($str, 16);
echo 'decimal value of $b = [', bi_to_str($b), "]<br/>\n";

echo 'binary value of $b = [', bi_to_str($b, 2), "]<br/>\n";

/*
    string bi_base_convert(string $num, int $base_from, int $base_to)

    Converts string representation of $num from base $base_from to $base_to
*/
$num = '-12345678900987654321';
echo 'Decimal number [', $num, '] equals to [', bi_base_convert($num, 10, 36), '] by base 36', "<br/>\n";

/*
    basic functions
*/
echo '<h3>basic functions</h3>' . "\n";
$c = bi_add($a, $b);
echo '$a + $b = [', bi_to_str($c), "]<br/>\n";
$c = bi_sub($a, $b);
echo '$a - $b = [', bi_to_str($c), "]<br/>\n";
$c = bi_mul($a, $b);
echo '$a * $b = [', bi_to_str($c), "]<br/>\n";
$c = bi_div($a, $b);
echo '$a / $b = [', bi_to_str($c), "]<br/>\n";
$c = bi_mod($a, $b);
echo '$a % $b = [', bi_to_str($c), "]<br/>\n";
$c = bi_abs($a);
echo 'abs($a) = [', bi_to_str($c), "]<br/>\n";
$c = bi_neg($a);
echo 'neg($a) = [', bi_to_str($c), "]<br/>\n";
$c = bi_inc($a);
echo 'inc($a) = [', bi_to_str($c), "]<br/>\n";
$c = bi_dec($a);
echo 'dec($a) = [', bi_to_str($c), "]<br/>\n";
$c = bi_sqr($a);
echo 'sqr($a) = [', bi_to_str($c), "]<br/>\n";
echo 'cmp($a, $b) = ', bi_cmp($a, $b), "<br/>\n";
echo 'cmp_abs($a, $b) = ', bi_cmp_abs($a, $b), "<br/>\n";
echo 'is_zero($a) = ', bi_is_zero($a) ? 'true' : 'false', "<br/>\n";
echo 'is_one($a) = ', bi_is_one($a) ? 'true' : 'false', "<br/>\n";
echo 'sign($a) = ', bi_sign($a), "<br/>\n";

/*
    bitset functions
*/
echo '<h3>bitset functions</h3>' . "\n";
echo '<div style="text-align:right">';
echo '$a = [', bi_to_str($a, 2), "]<br/>\n";
echo '$b = [', bi_to_str($b, 2), "]<br/>\n";
$c = bi_or($a, $b);
echo '$a or $b = [', bi_to_str($c, 2), "]<br/>\n";
$c = bi_xor($a, $b);
echo '$a xor $b = [', bi_to_str($c, 2), "]<br/>\n";
$c = bi_and($a, $b);
echo '$a and $b = [', bi_to_str($c, 2), "]<br/>\n";
$c = bi_andnot($a, $b);
echo '$a andnot $b = [', bi_to_str($c, 2), "]<br/>\n";
$c = bi_set_bit($a, 0);
echo 'set_bit($a, 0) = [', bi_to_str($c, 2), "]<br/>\n";
$c = bi_clr_bit($a, 0);
echo 'clr_bit($a, 0) = [', bi_to_str($c, 2), "]<br/>\n";
$c = bi_inv_bit($a, 0);
echo 'inv_bit($a, 0) = [', bi_to_str($c, 2), "]<br/>\n";
$c = bi_subint($a, 10, 20);
echo 'subint($a, 10, 20) = [', bi_to_str($c, 2), "]<br/>\n";
$c = bi_rshift($a, 10);
echo '$a >> 10 = [', bi_to_str($c, 2), "]<br/>\n";
$c = bi_lshift($a, 10);
echo '$a << 10 = [', bi_to_str($c, 2), "]<br/>\n";
echo '</div>', "<br/>\n";

echo 'Hamming distance($a, $b) = ', bi_hamming_distance($a, $b), "<br/>\n";
echo 'bit_len($a) = ', bi_bit_len($a), "<br/>\n";
echo 'bit1_cnt($a) = ', bi_bit1_cnt($a), "<br/>\n";
echo 'test_bit($a, 0) = ', bi_test_bit($a, 0), "<br/>\n";
echo 'scan0_bit($a, 0) = ', bi_scan0_bit($a, 0), "<br/>\n";
echo 'scan1_bit($a, 0) = ', bi_scan1_bit($a, 0), "<br/>\n";

echo '<h3>pseudorandom functions</h3>' . "\n";
/* 
    resource bi_rand(int $bit_len[, string $rand_func_name])
    Returns pseudorandom number with $bit_len bit length.
    Attention: do not use this function in cryptographic
    or security applications! The function can be used only
    for educational purposes :)
*/
$c = bi_rand(100);
echo 'rand(100) = ', bi_to_str($c), "<br/>\n";
$c = bi_rand(100, 'mt_rand'); // use mt_rand() as random generator
echo 'rand(100, "mt_rand") = ', bi_to_str($c), "<br/>\n";

/*
    functions for modular arithmetic calculations
*/
echo '<h3>modular arithmetic functions</h3>' . "\n";
// find next pseudoprime number after $c
$modulus = bi_next_prime($c);
echo '$modulus = next_prime($c) = [', bi_to_str($modulus), "]<br/>\n";

$c = bi_addmod($a, $b, $modulus);
echo '$a + $b (mod $modulus) = [', bi_to_str($c), "]<br/>\n";
$c = bi_submod($a, $b, $modulus);
echo '$a - $b (mod $modulus) = [', bi_to_str($c), "]<br/>\n";
$c = bi_mulmod($a, $b, $modulus);
echo '$a * $b (mod $modulus) = [', bi_to_str($c), "]<br/>\n";
$c = bi_divmod($a, $b, $modulus);
echo '$a / $b (mod $modulus) = [', bi_to_str($c), "]<br/>\n";
$c = bi_powmod($a, $b, $modulus);
echo 'pow($a, $b) (mod $modulus) = [', bi_to_str($c), "]<br/>\n";
$c = bi_factmod(1000, $modulus);
echo '1000! (mod $modulus) = [', bi_to_str($c), "]<br/>\n";
$c = bi_absmod(-1, $modulus);
echo '-1 (mod $modulus) = [', bi_to_str($c), "]<br/>\n";
$c = bi_invmod($a, $modulus);
echo '1 / $a (mod $modulus) = [', bi_to_str($c), "]<br/>\n";
$c = bi_sqrmod($a, $modulus);
echo 'sqr($a) (mod $modulus) = [', bi_to_str($c), "]<br/>\n";

echo 'cmp($a, $b) (mod $modulus) = ', bi_cmpmod($a, $b, $modulus), "<br/>\n";

/*
    other functions
*/
echo '<h3>other functions</h3>' . "\n";
/*
    attention: second parameter of bi_pow() must have 
    integer type (not BIG_INT!)
*/
$c = bi_pow($a, 4);
echo 'pow($a, 4) = [', bi_to_str($c), "]<br/>\n";
// argument of bi_fact() must have integer type (not BIG_INT)
$c = bi_fact(100);
echo '100! = [', bi_to_str($c), "]<br/>\n";

$c = bi_sqrt($a);
echo 'sqrt($a) = [', bi_to_str($c), "]<br/>\n";
$c = bi_sqrt_rem($a);
echo 'sqrt_rem($a) = [', bi_to_str($c), "]<br/>\n";

$c = bi_gcd($a, $b);
echo 'GCD($a, $b) = [', bi_to_str($c), "]<br/>\n";
// Miller-Rabin primality test of $a
echo 'miller_test($a, $b) = ', bi_miller_test($a, $b), "<br/>\n";
// primality test of $a
echo 'is_prime($a) = ', bi_is_prime($a), "<br/>\n";
// Jacobi symbol ($a|$b)
echo 'jacobi($a, $b) = ', bi_jacobi($a, $b), "<br/>\n";


$c = bi_div_extended($a, $b);
echo '$a = $b * ', bi_to_str($c[0]), ' + ', bi_to_str($c[1]), "<br/>\n";
$c = bi_gcd_extended($a, $b);
echo 'abs($a) * ', bi_to_str($c[1]), ' + abs($b) * ', bi_to_str($c[2]), ' = ', bi_to_str($c[0]), "<br/>\n";

// serialize $a into sequence of bytes
$str = bi_serialize($a);
echo '$str = serialize($a) = [', base64_encode($str), "]<br/>\n";
// unserialize $a from sequence of bytes
$a = bi_unserialize($str);
echo 'unserialize($str) = [', bi_to_str($a), "]<br/>\n";

// show package info
var_dump(bi_info());

echo 'the list of all functions in BIG_INT module:', "<br/>\n";
echo implode(", ", get_extension_funcs('big_int'));
echo "<br/>\n";

?>