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
/**
    This is an example of usage big_int extension

    RSA-like implementation of algorithm

    Attention: do not use this implementation in real life, because keys
    are generating with not very good "random generator". See microtime_generator()
    function below.
*/

require_once(dirname(__FILE__) . '/std_header.php');

define('KEY_LENGTH', 1024); // length of RSA keys

set_time_limit(0);

// generate RSA key pair
echo 'generating ' . KEY_LENGTH . '-bit key pair...';
$key_pair = generate_key_pair(KEY_LENGTH);
echo 'end' . "<br/>\n";
echo 'public key: [' . base64_encode($key_pair['public_key']) . ']' . "<br/>\n";
echo 'private key: [' . base64_encode($key_pair['private_key']) . ']' . "<br/>\n";
echo 'shared modulus: [' . base64_encode($key_pair['n']) . ']' . "<br/>\n";

// encrypt $text
$text = 'top secret message';
echo "plaintext [${text}]<br/>\n";
$enc_text = encrypt_text($text, $key_pair['public_key'], $key_pair['n']);
echo 'encrypted text: [' . base64_encode($enc_text) . "]<br/>\n";
// decrypt text
$plain_text = encrypt_text($enc_text, $key_pair['private_key'], $key_pair['n']);
echo "decrypted text [${plain_text}]<br>\n";

/************************************************************************/

/**
    encrypts / decrypts $text with key ($e, $n)
*/
function encrypt_text($text, $e, $n)
{
    $tmp = bi_unserialize($text);
    $e = bi_unserialize($e);
    $n = bi_unserialize($n);
    if (bi_cmp($tmp, $n) >= 0) {
        die('$text is too long to encrypt by key with length ' . bi_bit_len($n) . ' bits' . "<br/>\n");
    }
    return bi_serialize(bi_powmod($tmp, $e, $n));
}

/**
    returns RSA key pair with length $bit_len
*/
function generate_key_pair($bit_len)
{
    // generate two primes p and q
    $p_len = (int) ($bit_len / 2) + 1;
    $q_len = $bit_len - $p_len;
    $p = get_prime($p_len);
    $q = get_prime($q_len);
    // $n - is shared modulus
    $n = bi_mul($p, $q);
    // generate public ($e) and private ($d) keys
    $pq = bi_mul(bi_dec($p), bi_dec($q));
    do {
        $e = bi_rand($q_len, 'microtime_generator');
    } while (!bi_is_zero(bi_dec(bi_gcd($e, $pq))));
    $d = bi_invmod($e, $pq);
    return array(
        'n' => bi_serialize($n),
        'public_key' => bi_serialize($e),
        'private_key' => bi_serialize($d),
    );
}

/**
    returns a "good" prime $a with length $bit_len with
    resrtiction:
        $a - 1 != 0 (mod $e)
*/
function get_prime($bit_len)
{
    return bi_next_prime(bi_set_bit(bi_rand($bit_len, 'microtime_generator'), $bit_len - 1));
}

/**
    microtime() "random generator". It is cryptorgraphy-stronger
    than rand() from standard C library. But is is not good for serious usage :)
*/
function microtime_generator()
{
    $tmp = explode(" ", microtime());
    return (int) ($tmp[0] * 1000000); // use microseconds as more 'random' than seconds
}

?>