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
$out_file = './libbig_int.so';
@macro_dfn = (
    'BIG_INT_DIGIT_SIZE=32',
    'NDEBUG', # disable debugging
);
@libs = (
);
@options = (
    '-Wall',
    '-Wextra',
    '-Os', # size optimization
    '-shared', # create shared library
    '-fpic',
);
@include_dirs = (
    '../../../include',
);
@src_files = (
    '../../../src/low_level_funcs/*.c',
    '../../../src/*.c',
);

# generate options
$str = $compiler . ' ';
$str .= join(' ', @options);
$str .= ' ';
foreach (@libs) {
    $str .= '-l' . $_ . ' ';
}
foreach (@include_dirs) {
    $str .= '-I' . $_ . ' ';
}
foreach (@macro_dfn) {
    $str .= '-D' . $_ . ' ';
}
$str .= '-o ' . $out_file;
$str .= ' ';
$str .= join(' ', @src_files);

print "start of compilation...\n";
print "$str\n";
system($str);
print "end\n";
