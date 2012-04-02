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
$out_file = './calc32';
@macro_dfn = (
    'BIG_INT_DIGIT_SIZE=32',
    'NDEBUG',
);
@libs = (
);
@options = (
    '-Wall',
    '-Wextra',
    '-Werror',
    '-Os',
    '-s',
);
@include_dirs = (
    '../../../include',
);
@src_files = (
    '../../../src/low_level_funcs/*.c',
    '../../../src/*.c',
    '../calc.c',
);

print "start of configuration...\n";
# check presence of readline library
print "checking readline library...";
if (length(`locate libreadline.a`) && length(`locate libcurses.a`)) {
    print "found\n";
    push(@macro_dfn, 'HAVE_READLINE');
    push(@libs, 'readline', 'curses');
} else {
    print "not found\n";
}

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

print "end of configuration\n";
print "start of compilation...\n";
print "$str\n";
system($str);
print "end\n";
