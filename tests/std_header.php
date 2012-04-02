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

if (!extension_loaded('big_int')) {
    if (!@dl('big_int.' . PHP_SHLIB_SUFFIX) && !@dl('php_big_int.' . PHP_SHLIB_SUFFIX)) {
        die('cannot load big_int library' . "<br/>\n");
    }
}

?>