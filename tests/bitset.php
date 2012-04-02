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
    tests bitset functions (or, and, xor, andnot with start position displacement)
*/

require_once(dirname(__FILE__) . '/std_header.php');

$a = bi_from_str('1110110110110101011000100010110011101110111', 2);
$b = bi_from_str('11010010101111101010000101010100011101', 2);

echo '$a = ', bi_to_str($a, 2), "\n";
echo '$b = ', bi_to_str($b, 2), "\n";

////////////////////////////////////////////////
$c = bi_andnot($a, $b);
echo 'bi_andnot($a, $b) = ', bi_to_str($c, 2), "\n";

$c = bi_xor($a, $b);
echo 'bi_xor($a, $b) = ', bi_to_str($c, 2), "\n";

$c = bi_or($a, $b);
echo 'bi_or($a, $b) = ', bi_to_str($c, 2), "\n";

$c = bi_and($a, $b);
echo 'bi_and($a, $b) = ', bi_to_str($c, 2), "\n";

////////////////////////////////////////////////
$c = bi_andnot($a, $b, 2);
echo 'bi_andnot($a, $b, 2) = ', bi_to_str($c, 2), "\n";

$c = bi_xor($a, $b, 5);
echo 'bi_xor($a, $b, 5) = ', bi_to_str($c, 2), "\n";

$c = bi_or($a, $b, 32);
echo 'bi_or($a, $b, 32) = ', bi_to_str($c, 2), "\n";

$c = bi_or($a, $b, 33);
echo 'bi_or($a, $b, 33) = ', bi_to_str($c, 2), "\n";

$c = bi_or($a, $b, 70);
echo 'bi_or($a, $b, 70) = ', bi_to_str($c, 2), "\n";

$c = bi_and($a, $b, 7);
echo 'bi_and($a, $b, 7) = ', bi_to_str($c, 2), "\n";

////////////////////////////////////////////////
echo '$a = ', bi_to_str($a, 2), "\n";
echo '$b = ', bi_to_str($b, 2), "\n";


?>