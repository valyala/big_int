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
    example:
    usage of user function as random number generator
    for bi_rand() function.
    bi_rand() uses only lower byte of value, returned by
    user function.
*/

require_once(dirname(__FILE__) . '/std_header.php');


echo "const generator:<br>\n";
echo bi_to_str(bi_rand(100, 'const_generator'), 16), "<br>\n\n";

echo "time generator:<br>\n";
echo bi_to_str(bi_rand(100, 'time_generator'), 16), "<br>\n\n";

echo "microtime generator:<br>\n";
echo bi_to_str(bi_rand(100, 'microtime_generator'), 16), "<br>\n\n";

echo "static generator:<br>\n";
echo bi_to_str(bi_rand(100, 'static_generator'), 16), "<br>\n\n";

exit;

/***************************************************************************/
/**
    'constant' number generator
*/
function const_generator()
{
    return 0xff;
}

/**
    time() number generator
*/
function time_generator()
{
    //sleep(1);
    return time();
}

/**
    microtime() generator
*/
function microtime_generator()
{
    $tmp = explode(" ", microtime());
    return (int) ($tmp[0] * 1000000); // use microseconds as more 'random' than seconds ;)
}

/**
    generator with static varibale
*/
function static_generator()
{
    static $i = 0;
    return ++$i;
}

?>