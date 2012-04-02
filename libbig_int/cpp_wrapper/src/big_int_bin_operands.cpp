#include "big_int.hpp"

namespace BigIntNS
{

// constructor
BinOperands::BinOperands(bin_op_func f, const BigInt& op1, const BigInt& op2) : func(f), n1(op1.num), n2(op2.num) { }

// BigInt cast operator
BinOperands::operator const BigInt () const
{
    BigInt tmp;
    switch (func(n1, n2, tmp.num.create())) {
    case 0: return tmp; // no errors;
    case 1:
        if (func == BigIntAPI::big_int_div || func == BigIntAPI::big_int_mod) throw DivideByZeroError();
        // break skipped eventually
    default: throw MemoryError(); // unknown error
    }
}

// assign function
const BinOperands& BinOperands::assign(bin_op_func f, const BigInt& n, bool is_right) const
{
    if (is_right) {
        n1.assign(BigInt(*this).num);
        n2.assign(n.num);
    } else {
        n2.assign(BigInt(*this).num);
        n1.assign(n.num);
    }
    func = f;
    return *this;
}

BigInt BinOperands::operator - () const
{
    return BigInt(*this).neg();
}

} // namespace BigIntNS
