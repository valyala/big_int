#ifndef BIG_INT_BIN_OPERANDS_HPP
#define BIG_INT_BIN_OPERANDS_HPP

#include "big_int_consts.hpp"

namespace BigIntNS
{

// helper class
struct BinOperands
{
    mutable bin_op_func func;
    mutable BigIntContainer n1, n2;

    BinOperands(bin_op_func, const BigInt&, const BigInt&);
    const BinOperands& assign(bin_op_func, const BigInt&, bool is_right = true) const;
    operator const BigInt () const;

    BigInt operator - () const;

}; // class BinOperands

} // namespace BigIntNS

#endif // #ifndef BIG_INT_BIN_OPERANDS_HPP
