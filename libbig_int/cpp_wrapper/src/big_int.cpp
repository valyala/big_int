#include "big_int.hpp"

namespace BigIntNS {

/*****************************************************************************
 * public functions
 ****************************************************************************/

// converts BigInt number to string
std::string BigInt::toString(int base) const
{
    using namespace BigIntAPI;
    std::string str;
    big_int_str *s = big_int_str_create(1);

    if (!s) throw MemoryError();

    if (big_int_to_str(num, base, s)) {
        big_int_str_destroy(s);
        throw MemoryError();
    }

    try { str.assign(s->str, s->len); }
    catch (...) {
        big_int_str_destroy(s);
        throw;
    }
    big_int_str_destroy(s);
    return str;
}


// serializes BigInt number into string
std::string BigInt::serialize(bool is_save_sign) const
{
    using namespace BigIntAPI;
    std::string str;
    big_int_str *s = big_int_str_create(1);

    if (!s) throw MemoryError();

    if (big_int_serialize(num, is_save_sign, s)) {
        big_int_str_destroy(s);
        throw MemoryError();
    }

    try { str.assign(s->str, s->len); }
    catch (...) {
        big_int_str_destroy(s);
        throw;
    }
    big_int_str_destroy(s);
    return str;
}

// unserializes BigInt number from string [s]
BigInt& BigInt::unserialize(const std::string& s, bool is_restore_sign)
{
    using namespace BigIntAPI;
    big_int_str tmp_s;

    tmp_s.str = const_cast<char *>(s.c_str());
    tmp_s.len = s.length();

    if (big_int_unserialize(&tmp_s, is_restore_sign, num.create())) throw MemoryError();
    return *this;
}

/*****************************************************************************
 * private functions
 ****************************************************************************/
// Creates BigInt number from string [s] with length [s_len] with base [base]
void BigInt::_fromString(const char *s, size_t s_len, int base, size_t prealloc_len)
{
    using namespace BigIntAPI;
    big_int_str tmp_s;

    tmp_s.str = const_cast<char *>(s);
    tmp_s.len = s_len;

    switch (big_int_from_str(&tmp_s, base, num.create(prealloc_len))) {
    case 0: return; // no errors
    case 1: throw WrongBaseError(); // wrong base
    case 2: throw WrongCharError(); // string [s] contains wrong character for base [base]
    case 3: throw ShortStringError(); // length of [s] must be greater than 0
    default: throw MemoryError(); // unknown error (or memory error)
    }
}

std::string BigInt::_baseConvert(const char *s, size_t s_len, int from_base, int to_base)
{
    using namespace BigIntAPI;
    big_int_str tmp_s;
    std::string str;
    big_int_str *dst = big_int_str_create(1);

    if (!dst) throw MemoryError();

    tmp_s.str = const_cast<char *>(s);
    tmp_s.len = s_len;

    switch (big_int_base_convert(&tmp_s, dst, from_base, to_base)) {
    case 0: // no errors
        try { str.assign(dst->str, dst->len); }
        catch (...) {
            big_int_str_destroy(dst);
            throw;
        }
        big_int_str_destroy(dst);
        return str;
    case 1: // wrong from_base
    case 2: // wrong to_base
        throw WrongBaseError();
    case 3: throw WrongCharError(); // unexpected character in [s]
    case 4: throw ShortStringError(); // string [s] is too short
    default: throw MemoryError(); // unknown error (maybe memory error)
    }
}

} // namespase BigIntNS

