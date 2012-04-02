#ifndef BIG_INT_IOSTREAM_HPP
#define BIG_INT_IOSTREAM_HPP
#include "big_int.hpp"
#include <istream>
#include <ostream>

namespace BigIntNS
{

inline std::ostream& operator << (std::ostream &out, const BigInt& n)
{
    return out << n.toString();
}

inline std::istream& operator >> (std::istream &in, BigInt& n)
{
    std::string tmp;

    in >> tmp;
    n.fromString(tmp);
    return in;
}

} // namespace BigIntNS

#endif
