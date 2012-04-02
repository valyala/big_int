PHP_ARG_ENABLE(big-int, big_int module,
[  --enable-big-int          Enable big_int module.])

if test "$PHP_BIG_INT" != "no"; then
    PHP_NEW_EXTENSION(big_int,
        php_big_int.c \
        libbig_int/src/memory_manager.c \
        libbig_int/src/basic_funcs.c \
        libbig_int/src/bitset_funcs.c \
        libbig_int/src/modular_arithmetic.c \
        libbig_int/src/number_theory.c \
        libbig_int/src/service_funcs.c \
        libbig_int/src/str_funcs.c \
        libbig_int/src/low_level_funcs/add.c \
        libbig_int/src/low_level_funcs/and.c \
        libbig_int/src/low_level_funcs/andnot.c \
        libbig_int/src/low_level_funcs/cmp.c \
        libbig_int/src/low_level_funcs/div.c \
        libbig_int/src/low_level_funcs/mul.c \
        libbig_int/src/low_level_funcs/or.c \
        libbig_int/src/low_level_funcs/sqr.c \
        libbig_int/src/low_level_funcs/sub.c \
        libbig_int/src/low_level_funcs/xor.c,
        $ext_shared, , -I@ext_srcdir@/libbig_int/include)
    PHP_ADD_BUILD_DIR($ext_builddir/libbig_int/src)
    PHP_ADD_BUILD_DIR($ext_builddir/libbig_int/src/low_level_funcs)
    AC_DEFINE(HAVE_BIG_INT, 1, [Have BIG_INT library])
fi
