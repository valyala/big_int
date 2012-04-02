/**
    constants and types for BigInt classes
*/
#ifndef BIG_INT_CONSTS_HPP
#define BIG_INT_CONSTS_HPP

#include <string> // for string class
#include <cstring> // for std::strlen()
#include <cstdlib> // for std::rand() function

#include "big_int_exceptions.hpp"

namespace BigIntNS
{

using std::size_t;

// all big_int C API is hidden in BigIntNS::BIG_INT_API namespace
namespace BigIntAPI
{
#include "big_int_full.h"
}

// default size of created number
const static size_t default_prealloc_len = 1;
// default base, used in conversion to/from string
const static int default_base = 10;

// helper types for pointers to functions from big_int C library
typedef int (*un_op_func)(const BigIntAPI::big_int *, BigIntAPI::big_int *);
typedef bool (*un_op_func1)(int);
typedef int (*bin_op_func)(const BigIntAPI::big_int *, const BigIntAPI::big_int *, BigIntAPI::big_int *);
typedef int (*bin_op_func1)(const BigIntAPI::big_int *, const BigIntAPI::big_int *, size_t, BigIntAPI::big_int *);
typedef int (*bin_op_func2)(const BigIntAPI::big_int *, size_t, BigIntAPI::big_int *);
typedef int (*tri_op_func)(const BigIntAPI::big_int *, const BigIntAPI::big_int *, const BigIntAPI::big_int *, BigIntAPI::big_int *);

// classes
class BigInt;
struct BinOperands;

} // namespace BigIntNS

#endif // #ifndef BIG_INT_CONSTS_HPP
