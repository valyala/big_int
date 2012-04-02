#!/usr/bin/perl -w
########################################################################
#    Copyright 2004, 2005 Alexander Valyalkin
#
#    These sources is free software. You can redistribute it and/or
#    modify it freely. You can use it with any free or commercial
#    software.
#
#    These sources is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY. Without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
#    You may contact the author by:
#       e-mail:  valyala@gmail.com
########################################################################

$compiler = 'gcc';
$out_file = './test1_32';
@macro_dfn = (
    'BIG_INT_DIGIT_SIZE=32',
    'NDEBUG',
);
@options = (
    '-Wall',
    '-Wextra',
    '-Os', # optimization
);
@include_dirs = (
    '../../../include',
);
@src_files = (
    '../../../src/low_level_funcs/*.c',
    '../../../src/*.c',
    '../test1.c',
);

# generate options
$str = $compiler . ' ';
$str .= join(' ', @options);
$str .= ' ';
foreach (@include_dirs) {
    $str .= '-I' . $_ . ' ';
}
foreach (@macro_dfn) {
    $str .= '-D' . $_ . ' ';
}
$str .= '-o ' . $out_file;
$str .= ' ';
$str .= join(' ', @src_files);

print "start of compiling...\n";
system($str);
print "end\n";
