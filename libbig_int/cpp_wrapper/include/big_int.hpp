/**
    wrapper for big_int C library
*/
#ifndef BIG_INT_HPP
#define BIG_INT_HPP

#include "big_int_consts.hpp"
#include "big_int_container.hpp"
#include "big_int_bin_operands.hpp"

namespace BigIntNS
{

/**
    class declaration BigInt.
*/
class BigInt
{
private:
    /*****************************************************************************
     * BigInt private members
     ****************************************************************************/
    BigIntContainer num; // container for big_int number

    // converts number from string into BigInt number
    void _fromString(const char *s, size_t s_len, int base, size_t prealloc_len);
    // converts base of number
    static std::string _baseConvert(const char *s, size_t s_len, int from_base, int to_base);

    // helper functions
    BigInt& _unFunc(un_op_func func)
    {
        const BigIntAPI::big_int *src = num;
        if (func(src, num.create())) throw MemoryError();
        return *this;
    }

    BigInt& _binFunc(bin_op_func func, const BigInt& n)
    {
        using namespace BigIntAPI;
        const big_int *src1 = num, *src2 = n.num;
        switch (func(src1, src2, num.create())) {
        case 0: return *this; // no errors
        case 1:
            if (func == big_int_div || func == big_int_mod) throw DivideByZeroError();
            // break skipped eventually
        default: throw MemoryError(); // unknown error
        }
    }

    BigInt& _binFunc1(bin_op_func1 func, const BigInt& n, size_t start_pos = 0)
    {
        const BigIntAPI::big_int *src1 = num, *src2 = n.num;
        if (func(src1, src2, start_pos, num.create())) throw MemoryError();
        return *this;
    }

    BigInt& _binFunc2(bin_op_func2 func, size_t pos)
    {
        const BigIntAPI::big_int *src = num;
        if (func(src, pos, num.create())) throw MemoryError();
        return *this;
    }

    // helper function for big_int_*mod() functions
    BigInt& _triFunc(tri_op_func func, const BigInt& n, const BigInt& mod)
    {
        using namespace BigIntAPI;
        const big_int *src1 = num, *src2 = n.num, *src3 = mod.num;
        switch (func(src1, src2, src3, num.create())) {
        case 0: return *this; // all ok
        case 1: throw DivideByZeroError();
        case 2: throw InvNumberError();
        default: throw MemoryError();
        }
    }

    static int _bitOr(const BigIntAPI::big_int *n1, const BigIntAPI::big_int *n2, BigIntAPI::big_int *ans)
    {
        return BigIntAPI::big_int_or(n1, n2, 0, ans);
    }

    static int _bitAnd(const BigIntAPI::big_int *n1, const BigIntAPI::big_int *n2, BigIntAPI::big_int *ans)
    {
        return BigIntAPI::big_int_and(n1, n2, 0, ans);
    }

    static int _bitXor(const BigIntAPI::big_int *n1, const BigIntAPI::big_int *n2, BigIntAPI::big_int *ans)
    {
        return BigIntAPI::big_int_xor(n1, n2, 0, ans);
    }

    static bool _cmp(un_op_func1 cmp_func, const BigIntAPI::big_int *n1, const BigIntAPI::big_int *n2)
    {
        int cmp_flag;
        BigIntAPI::big_int_cmp(n1, n2, &cmp_flag);
        return cmp_func(cmp_flag);
    }

    //  helper classes
    static bool _eq(int cmp_flag) { return cmp_flag == 0; }
    static bool _ne(int cmp_flag) { return cmp_flag != 0; }
    static bool _ge(int cmp_flag) { return cmp_flag >= 0; }
    static bool _gt(int cmp_flag) { return cmp_flag > 0; }
    static bool _le(int cmp_flag) { return cmp_flag <= 0; }
    static bool _lt(int cmp_flag) { return cmp_flag < 0; }

public:
    // BinOperands class must have access to private members of BigInt class
    friend struct BinOperands;

    /*****************************************************************************
     * BigInt constructors & destructor
     ****************************************************************************/
    // create number from integer
    BigInt(int n = 0, size_t prealloc_len = default_prealloc_len) : num(prealloc_len) { fromInt(n); }

    // create number from const char *
    BigInt(const char *s, int base = default_base, size_t prealloc_len = default_prealloc_len) : num(prealloc_len)
    {
        _fromString(s, std::strlen(s), base, prealloc_len);
    }

    // create number from string
    BigInt(const std::string& s, int base = default_base, size_t prealloc_len = default_prealloc_len) : num(prealloc_len)
    {
        _fromString(s.c_str(), s.length(), base, prealloc_len);
    }

    // copy constructor
    BigInt(const BigInt& n) : num(n.num) { }

    // destructor (yes, it is empty!)
    ~BigInt() { }

    /*****************************************************************************
     * BigInt copy operators
     ****************************************************************************/
    // main assign operators
    BigInt& operator = (const BigInt& n) { return assign(n); }
    BigInt& operator = (int n) { return assign(n); }
    BigInt& operator = (const char *n) { return assign(n); }
    BigInt& operator = (const std::string& n) { return assign(n); }

    // trenary operator (result = n1 op n2)
    BigInt& operator = (const BinOperands& a)
    {
        using namespace BigIntAPI;
        const big_int *src1 = a.n1, *src2 = a.n2;
        switch (a.func(src1, src2, num.create())) {
        case 0: return *this; // no errors
        case 1:
            if (a.func == big_int_div || a.func == big_int_mod) throw DivideByZeroError();
            // break skipped eventually
        default: throw MemoryError(); // unknown error
        }
    }

    /*****************************************************************************
     * BigInt assign arithmetic operators
     ****************************************************************************/
    BigInt& operator += (const BigInt& n) { return _binFunc(BigIntAPI::big_int_add, n); }
    BigInt& operator -= (const BigInt& n) { return _binFunc(BigIntAPI::big_int_sub, n); }
    BigInt& operator *= (const BigInt& n) { return _binFunc(BigIntAPI::big_int_mul, n); }
    BigInt& operator /= (const BigInt& n) { return _binFunc(BigIntAPI::big_int_div, n); }
    BigInt& operator %= (const BigInt& n) { return _binFunc(BigIntAPI::big_int_mod, n); }

    /*****************************************************************************
     * BigInt assign bitset operators
     ****************************************************************************/
    // bitset assign operators
    BigInt& operator |= (const BigInt& n) { return _binFunc1(BigIntAPI::big_int_or, n); }
    BigInt& operator &= (const BigInt& n) { return _binFunc1(BigIntAPI::big_int_and, n); }
    BigInt& operator ^= (const BigInt& n) { return _binFunc1(BigIntAPI::big_int_xor, n); }
    BigInt& operator >>= (int n) { return rShift(n); }
    BigInt& operator <<= (int n) { return lShift(n); }

    /*****************************************************************************
     * BigInt unary operators
     ****************************************************************************/
    // prefix increment operator
    BigInt& operator ++ () { return inc(); }
    // prefix decrement operator
    BigInt& operator -- () { return dec(); }

    // postfix increment
    const BigInt operator ++ (int)
    {
        using namespace BigIntAPI;
        const big_int *src = num;
        BigInt tmp = *this;
        if (big_int_inc(src, num.create())) throw MemoryError();
        return tmp;
    }

    // postfix decrement
    const BigInt operator -- (int)
    {
        using namespace BigIntAPI;
        const big_int *src = num;
        BigInt tmp = *this;
        if (big_int_dec(src, num.create())) throw MemoryError();
        return tmp;
    }

    // unary minus
    const BigInt operator - () const { return clone().neg(); }

    /*****************************************************************************
     * BigInt binary comparision operators
     ****************************************************************************/
    // comparision operators
    friend bool operator == (const BigInt& n1, const BigInt& n2) { return _cmp(_eq, n1.num, n2.num); }
    friend bool operator != (const BigInt& n1, const BigInt& n2) { return _cmp(_ne, n1.num, n2.num); }
    friend bool operator >= (const BigInt& n1, const BigInt& n2) { return _cmp(_ge, n1.num, n2.num); }
    friend bool operator <= (const BigInt& n1, const BigInt& n2) { return _cmp(_le, n1.num, n2.num); }
    friend bool operator > (const BigInt& n1, const BigInt& n2) { return _cmp(_gt, n1.num, n2.num); }
    friend bool operator < (const BigInt& n1, const BigInt& n2) { return _cmp(_lt, n1.num, n2.num); }

    /*****************************************************************************
     * BigInt binary arithmetic operators
     ****************************************************************************/
    // binary operators
    friend BinOperands operator + (const BigInt& n1, const BigInt& n2) { return BinOperands(BigIntAPI::big_int_add, n1, n2); }
    // these two functions needed for faster calculations (n1 + n2), where n1 or n2 is BinOperands
    friend const BinOperands& operator + (const BigInt& n, const BinOperands& a) { return a.assign(BigIntAPI::big_int_add, n); }
    friend const BinOperands& operator + (const BinOperands& a, const BigInt& n) { return a.assign(BigIntAPI::big_int_add, n); }

    friend BinOperands operator - (const BigInt& n1, const BigInt& n2) { return BinOperands(BigIntAPI::big_int_sub, n1, n2); }
    // these two functions needed for faster calculations (n1 - n2), where n1 or n2 is BinOperands
    friend const BinOperands& operator - (const BigInt& n, const BinOperands& a) { return a.assign(BigIntAPI::big_int_sub, n, false); }
    friend const BinOperands& operator - (const BinOperands& a, const BigInt& n) { return a.assign(BigIntAPI::big_int_sub, n); }

    friend BinOperands operator * (const BigInt& n1, const BigInt& n2) { return BinOperands(BigIntAPI::big_int_mul, n1, n2); }
    // these two functions needed for faster calculations (n1 * n2), where n1 or n2 is BinOperands
    friend const BinOperands& operator * (const BigInt& n, const BinOperands& a) { return a.assign(BigIntAPI::big_int_mul, n); }
    friend const BinOperands& operator * (const BinOperands& a, const BigInt& n) { return a.assign(BigIntAPI::big_int_mul, n); }

    friend BinOperands operator / (const BigInt& n1, const BigInt& n2) { return BinOperands(BigIntAPI::big_int_div, n1, n2); }
    // these two functions needed for faster calculations (n1 / n2), where n1 or n2 is BinOperands
    friend const BinOperands& operator / (const BigInt& n, const BinOperands& a) { return a.assign(BigIntAPI::big_int_div, n, false); }
    friend const BinOperands& operator / (const BinOperands& a, const BigInt& n) { return a.assign(BigIntAPI::big_int_div, n); }

    friend BinOperands operator % (const BigInt& n1, const BigInt& n2) { return BinOperands(BigIntAPI::big_int_mod, n1, n2); }
    // these two functions needed for faster calculations (n1 % n2), where n1 or n2 is BinOperands
    friend const BinOperands& operator % (const BigInt& n, const BinOperands& a) { return a.assign(BigIntAPI::big_int_mod, n, false); }
    friend const BinOperands& operator % (const BinOperands& a, const BigInt& n) { return a.assign(BigIntAPI::big_int_mod, n); }

    /*****************************************************************************
     * BigInt binary bitset operators
     ****************************************************************************/
    // binary bitset operators
    friend BinOperands operator | (const BigInt& n1, const BigInt& n2) { return BinOperands(_bitOr, n1, n2); }
    // these two functions needed for faster calculations (n1 | n2), where n1 or n2 is BinOperands
    friend const BinOperands& operator | (const BigInt& n, const BinOperands& a) { return a.assign(_bitOr, n); }
    friend const BinOperands& operator | (const BinOperands& a, const BigInt& n) { return a.assign(_bitOr, n); }

    friend BinOperands operator & (const BigInt& n1, const BigInt& n2) { return BinOperands(_bitAnd, n1, n2); }
    // these two functions needed for faster calculations (n1 & n2), where n1 or n2 is BinOperands
    friend const BinOperands& operator & (const BigInt& n, const BinOperands& a) { return a.assign(_bitAnd, n); }
    friend const BinOperands& operator & (const BinOperands& a, const BigInt& n) { return a.assign(_bitAnd, n); }

    friend BinOperands operator ^ (const BigInt& n1, const BigInt& n2) { return BinOperands(_bitXor, n1, n2); }
    // these two functions needed for faster calculations (n1 ^ n2), where n1 or n2 is BinOperands
    friend const BinOperands& operator ^ (const BigInt& n, const BinOperands& a) { return a.assign(_bitXor, n); }
    friend const BinOperands& operator ^ (const BinOperands& a, const BigInt& n) { return a.assign(_bitXor, n); }

    const BigInt operator >> (int n) const
    {
        BigInt tmp;
        if (BigIntAPI::big_int_rshift(num, n, tmp.num.create())) throw MemoryError();
        return tmp;
    }

    const BigInt operator << (int n) const
    {
        BigInt tmp;
        if (BigIntAPI::big_int_lshift(num, n, tmp.num.create())) throw MemoryError();
        return tmp;
    }

    friend const BigInt operator >> (const BinOperands& a, int n) { return BigInt(a) >> n; }
    friend const BigInt operator << (const BinOperands& a, int n) { return BigInt(a) << n; }

    /*****************************************************************************
     * BigInt bitset functions
     ****************************************************************************/
    BigInt& bitOr(const BigInt& n, size_t start_pos = 0)
    {
        return _binFunc1(BigIntAPI::big_int_or, n, start_pos);
    }

    BigInt& bitAnd(const BigInt& n, size_t start_pos = 0)
    {
        return _binFunc1(BigIntAPI::big_int_and, n, start_pos);
    }

    BigInt& bitXor(const BigInt& n, size_t start_pos = 0)
    {
        return _binFunc1(BigIntAPI::big_int_xor, n, start_pos);
    }

    BigInt& bitAndNot(const BigInt& n, size_t start_pos = 0)
    {
        return _binFunc1(BigIntAPI::big_int_andnot, n, start_pos);
    }

    // shifts [this] to the right by [n] bits
    BigInt& rShift(int n)
    {
        using namespace BigIntAPI;
        const big_int *src = num;
        if (big_int_rshift(src, n, num.create())) throw MemoryError();
        return *this;
    }

    // shifts [this] to the left by [n] bits
    BigInt& lShift(int n)
    {
        using namespace BigIntAPI;
        const big_int *src = num;
        if (big_int_lshift(src, n, num.create())) throw MemoryError();
        return *this;
    }

    // returns length of [this] in bits
    unsigned int bitLength() const
    {
        unsigned int len;
        BigIntAPI::big_int_bit_length(num, &len);
        return len;
    }

    // calculates 1-bits number in [this]
    unsigned int bit1Cnt() const
    {
        unsigned int cnt;
        BigIntAPI::big_int_bit1_cnt(num, &cnt);
        return cnt;
    }

    // cuts part of [this], starting at [start_pos] with length [len]
    // if [is_invert] == true, then invert all bits in the result
    BigInt& subInt(size_t start_pos, size_t len, bool is_invert = false)
    {
        using namespace BigIntAPI;
        const big_int *src = num;
        if (big_int_subint(src, start_pos, len, is_invert, num.create())) throw MemoryError();
        return *this;
    }

    // sets to 1 bit number [pos] in [this]
    BigInt& setBit(size_t pos) { return _binFunc2(BigIntAPI::big_int_set_bit, pos); }

    // clears to 0 bit number [pos] in [this]
    BigInt& clrBit(size_t pos) { return _binFunc2(BigIntAPI::big_int_clr_bit, pos); }

    // inverts bit number [pos] in [this]
    BigInt& invBit(size_t pos) { return _binFunc2(BigIntAPI::big_int_inv_bit, pos); }

    // returns true, if bit number [pos] = 1, else returns false
    bool testBit(size_t pos) const
    {
        int flag;
        BigIntAPI::big_int_test_bit(num, pos, &flag);
        return flag == 1;
    }

    // returns position of first occurence of 1-bit in [this] starting at [start_pos]
    // throws OverflowError(), if can't find 1-bit
    size_t scan1Bit(size_t start_pos = 0) const
    {
        size_t pos;
        if (BigIntAPI::big_int_scan1_bit(num, start_pos, &pos)) throw OverflowError();
        return pos;
    }

    // returns position of first occurence of 0-bit in [this] starting at [start_pos]
    size_t scan0Bit(size_t start_pos = 0) const
    {
        size_t pos;
        BigIntAPI::big_int_scan0_bit(num, start_pos, &pos);
        return pos;
    }

    // calculates Hamming distance between [this] and [n]
    unsigned int hammingDistance(const BigInt& n) const
    {
        using namespace BigIntAPI;
        const big_int *src1 = num, *src2 = n.num;
        unsigned int dist;
        if (big_int_hamming_distance(src1, src2, &dist)) throw MemoryError();
        return dist;
    }

    // generates random number with length [bit_len], using random generator [func]
    BigInt& rand(size_t bit_len, BigIntAPI::big_int_rnd_fp func = std::rand)
    {
        if (BigIntAPI::big_int_rand(func, bit_len, num.create(bit_len / BIG_INT_DIGIT_SIZE))) throw MemoryError();
        return *this;
    }

    /*****************************************************************************
     * BigInt number theory functions
     ****************************************************************************/
    // calculates greatest common divider of [this] and [n]
    BigInt& GCD(const BigInt& n) { return _binFunc(BigIntAPI::big_int_gcd, n); }

    // calculates greatest common divider of [this] and [n]
    // and [x] with [y] such as abs(this)*x + abs(n)*y = gcd
    BigInt& GCDExt(const BigInt& n, BigInt& x, BigInt& y)
    {
        using namespace BigIntAPI;
        const big_int *src1 = num, *src2 = n.num;
        switch (big_int_gcd_extended(src1, src2, num.create(),
            x.num.create(), y.num.create())) {
        case 0: return *this; // all ok
        case 1: throw DivideByZeroError(); // division by zero. [this] and [n] cannot be zero
        default: throw MemoryError();
        }
    }

    // powers [this] int [n]
    BigInt& pow(int n)
    {
        using namespace BigIntAPI;
        const big_int *src = num;
        if (big_int_pow(src, n, num.create())) throw MemoryError();
        return *this;
    }

    // calculates square root of [this]
    // throws NegativeNumberError(), if [this] < 0
    BigInt& sqrt()
    {
        using namespace BigIntAPI;
        const big_int *src = num;
        switch (big_int_sqrt(src, num.create())) {
        case 0: return *this; // all ok
        case 1: throw NegativeNumberError();
        default: throw MemoryError();
        }
    }

    // calculates remainder of square root of [this]
    BigInt& sqrtRem()
    {
        using namespace BigIntAPI;
        const big_int *src = num;
        switch (big_int_sqrt_rem(src, num.create())) {
        case 0: return *this; // all ok
        case 1: throw NegativeNumberError();
        default: throw MemoryError();
        }
    }

    // calculates factorial of [n]
    // throws NegativeNumberError() exception, when [this] < 0
    BigInt& fact(int n)
    {
        switch (BigIntAPI::big_int_fact(n, num.create())) {
        case 0: return *this; //all ok
        case 1: throw NegativeNumberError();
        default: throw MemoryError();
        }
    }

    // preforms Miller test for [this] with base [base].
    // returns true, if [this] is pseudoprime by base [base]. else returns false
    // throws WrongBaseError() if unexpected base passed to function
    bool millerTest(const BigInt& base) const
    {
        int flag;
        switch (BigIntAPI::big_int_miller_test(num, base.num, &flag)) {
        case 0: return flag != 0; // all ok
        case 1: case 2: throw WrongBaseError(); // base is too small or too high. Iit must be 1 < base < (this - 1)
        default: throw MemoryError();
        }
    }

    // returns 0, if [this] is composite
    //         1, if [this] is strong pseudoprime 
    //         2, if [this] is proven prime
    int isPrime(unsigned int primes_to = 100, int level = 1) const
    {
        int flag;
        if (BigIntAPI::big_int_is_prime(num, primes_to, level, &flag)) throw MemoryError();
        return flag;
    }

    // calculates next prime number after [this]
    BigInt& nextPrime()
    {
        using namespace BigIntAPI;
        const big_int *src = num;
        if (BigIntAPI::big_int_next_prime(src, num.create())) throw MemoryError();
        return *this;
    }

    // calculates Jacobi symbol
    int jacobi(const BigInt& n) const
    {
        int j;
        switch (BigIntAPI::big_int_jacobi(num, n.num, &j)) {
        case 0: return j; // no errors
        case 1: throw EvenNumberError(); // [n] must be odd
        default: throw MemoryError();
        }
    }

    /*****************************************************************************
     * BigInt modular arithmetic functions
     ****************************************************************************/
    // calculates [this] + [n] (mod [mod])
    BigInt& addMod(const BigInt& n, const BigInt& mod) { return _triFunc(BigIntAPI::big_int_addmod, n, mod); }
    // calculates [this] - [n] (mod [mod])
    BigInt& subMod(const BigInt& n, const BigInt& mod) { return _triFunc(BigIntAPI::big_int_submod, n, mod); }
    // calculates [this] * [n] (mod [mod])
    BigInt& mulMod(const BigInt& n, const BigInt& mod) { return _triFunc(BigIntAPI::big_int_mulmod, n, mod); }
    // calculates [this] * invMod([n], [mod]) (mod [mod])
    BigInt& divMod(const BigInt& n, const BigInt& mod) { return _triFunc(BigIntAPI::big_int_divmod, n, mod); }
    // calculates pow([this], [n]) (mod [mod])
    BigInt& powMod(const BigInt& n, const BigInt& mod) { return _triFunc(BigIntAPI::big_int_powmod, n, mod); }

    // calculates fact([this]) (mod [mod])
    BigInt& factMod(const BigInt& mod) { return _binFunc(BigIntAPI::big_int_factmod, mod); }
    // calculates abs([this]) (mod [mod])
    BigInt& absMod(const BigInt& mod) { return _binFunc(BigIntAPI::big_int_absmod, mod); }
    // calculates inv([this]) (mod [mod])
    BigInt& invMod(const BigInt& mod) { return _binFunc(BigIntAPI::big_int_invmod, mod); }
    // calculates sqr([this]) (mod [mod])
    BigInt& sqrMod(const BigInt& mod) { return _binFunc(BigIntAPI::big_int_sqrmod, mod); }

    // compares [this] with [n] by modulus [mod]
    // returns -1, if this < n (mod [mod])
    //          0, if this = n (mod [mod])
    //          1, if this > n (mod [mod])
    int cmpMod(const BigInt& n, const BigInt& mod) const
    {
        int flag;
        switch (BigIntAPI::big_int_cmpmod(num, n.num, mod.num, &flag)) {
        case 0: return flag; // all ok
        case 1: throw DivideByZeroError();
        default: throw MemoryError();
        }
    }

    /*****************************************************************************
     * BigInt service functions
     ****************************************************************************/
    BigInt& assign(const BigInt& n) { num.assign(n.num); return *this; }
    BigInt& assign(int n, size_t prealloc_len = default_prealloc_len) { return fromInt(n, prealloc_len); }

    BigInt& assign(const char *s, int base = default_base, size_t prealloc_len = default_prealloc_len)
    {
        return fromString(s, base, prealloc_len);
    }

    BigInt& assign(const std::string& s, int base = default_base, size_t prealloc_len = default_prealloc_len)
    {
        return fromString(s, base, prealloc_len);
    }

    // reserves [prealloc_len] words for number [this]
    void reserve(size_t prealloc_len) { num.reserve(prealloc_len); }

    // returns length of [this] in words (see bitLength() also)
    size_t length() const { return num->len; }
    size_t size() const { return num->len; }

    // returns length of reserved memory
    size_t capacity() const { return num->len_allocated; }

    // assigns number form integer [n]
    BigInt& fromInt(int n, size_t prealloc_len = default_prealloc_len)
    {
        if (BigIntAPI::big_int_from_int(n, num.create(prealloc_len))) throw MemoryError();
        return *this;
    }

    // assigns number from const char *s
    BigInt& fromString(const char *s, int base = default_base, size_t prealloc_len = default_prealloc_len)
    {
        _fromString(s, std::strlen(s), base, prealloc_len);
        return *this;
    }

    // assigns number from string
    BigInt& fromString(const std::string& s, int base = default_base, size_t prealloc_len = default_prealloc_len)
    {
        _fromString(s.c_str(), s.length(), base, prealloc_len);
        return *this;
    }

    // converts BigInt number to int
    int toInt() const
    {
        int tmp;
        if (BigIntAPI::big_int_to_int(num, &tmp)) throw OverflowError();
        return tmp;
    }

    // converts BigInt number to string
    std::string toString(int base = default_base) const;

    // creates copy of number. Can be used to create "not const" value from "const"
    BigInt clone() const { return *this; }

    // serialization/deserialization functions
    std::string serialize(bool is_save_sign = false) const;
    BigInt& unserialize(const std::string& s, bool is_restore_sign = false);

    // returns version of BIG_INT libraray
    static const char* getVersion() { return BigIntAPI::big_int_version(); }

    // returns build date of BIG_INT library
    static const char* getBuildDate() { return BigIntAPI::big_int_build_date(); }

    // converts base
    static std::string baseConvert(const char *s, int from_base, int to_base = default_base)
    {
        return _baseConvert(s, std::strlen(s), from_base, to_base);
    }

    static std::string baseConvert(const std::string& s, int from_base, int to_base = default_base)
    {
        return _baseConvert(s.c_str(), s.length(), from_base, to_base);
    }

    /*****************************************************************************
     * BigInt arithmetic functions
     ****************************************************************************/
    BigInt& inc()
    {
        using namespace BigIntAPI;
        const big_int *src = num;
        if (big_int_inc(src, num.create())) throw MemoryError();
        return *this;
    }

    BigInt& dec()
    {
        using namespace BigIntAPI;
        const big_int *src = num;
        if (big_int_dec(src, num.create())) throw MemoryError();
        return *this;
    }

    BigInt& add(const BigInt& n) { return _binFunc(BigIntAPI::big_int_add, n); }
    BigInt& sub(const BigInt& n) { return _binFunc(BigIntAPI::big_int_sub, n); }
    BigInt& mul(const BigInt& n) { return _binFunc(BigIntAPI::big_int_mul, n); }
    BigInt& div(const BigInt& n) { return _binFunc(BigIntAPI::big_int_div, n); }
    BigInt& mod(const BigInt& n) { return _binFunc(BigIntAPI::big_int_mod, n); }

    // calculates this + n1 * n2;
    BigInt& addMul(const BigInt& n1, const BigInt& n2)
    {
        using namespace BigIntAPI;
        const big_int *src1 = n1.num, *src2 = n2.num, *src3 = num;
        if (big_int_muladd(src1, src2, src3, num.create())) throw MemoryError();
        return *this;
    }

    // calculates quotient and reminder of this / n;
    // quotient will be saved in this, reminder will be saved in r
    BigInt& divExt(const BigInt& n, BigInt& r)
    {
        using namespace BigIntAPI;
        const big_int *src1 = num, *src2 = n.num;
        switch (BigIntAPI::big_int_div_extended(src1, src2, num.create(), r.num.create())) {
        case 0: return *this; // no errors
        case 1: throw DivideByZeroError(); // division by zero
        default: throw MemoryError(); // unknown error (or memory error)
        }
    }

    // this = abs(this)
    BigInt& abs() { return _unFunc(BigIntAPI::big_int_abs); }
    // this = - this;
    BigInt& neg() { return _unFunc(BigIntAPI::big_int_neg); }
    // this = this * this;
    BigInt& sqr() { return _unFunc(BigIntAPI::big_int_sqr); }

    /*****************************************************************************
     * BigInt comparision functions
     ****************************************************************************/
    // compares [this] with [n]. Returns:
    //  -1, if this < n
    //   0, if this = n
    //   1, if this > n
    int cmp(const BigInt& n) const
    {
        int cmp_flag;
        BigIntAPI::big_int_cmp(num, n.num, &cmp_flag);
        return cmp_flag;
    }

    // compares abs(this) with abs(n). Returns:
    //  -1, if abs(this) < abs(n)
    //   0, if abs(this) = abs(n)
    //   1, if abs(this) > abs(n)
    int cmpAbs(const BigInt& n) const
    {
        int cmp_flag;
        BigIntAPI::big_int_cmp_abs(num, n.num, &cmp_flag);
        return cmp_flag;
    }

    /*****************************************************************************
     * BigInt other functions
     ****************************************************************************/
    // returns:
    //    -1, if [this] < 0
    //     0, if [this] = 0
    //     1, if [this] > 0
    int sign() const
    {
        using namespace BigIntAPI;
        sign_type sign;
        big_int_sign(num, &sign);
        return sign == PLUS ? (isZero() ? 0 : 1) : -1;
    }

    // returns true, if [this] =0, else returns false
    bool isZero() const
    {
        int flag;
        BigIntAPI::big_int_is_zero(num, &flag);
        return flag != 0;
    }

    // returns true, if [this] = 1, else returns false
    bool isOne() const
    {
        int flag;
        BigIntAPI::big_int_is_one(num, &flag);
        return flag != 0;
    }

}; // class BigInt

} // namespace BigIntNS

#endif // #ifndef BIG_INT_HPP
