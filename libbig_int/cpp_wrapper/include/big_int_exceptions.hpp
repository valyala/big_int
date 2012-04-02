/**
    exceptions for BigInt library
*/
#ifndef BIG_INT_EXCEPTIONS_HPP
#define BIG_INT_EXCEPTIONS_HPP

#include <stdexcept> /* for std::exception */

// BigInt exception classes declaration
namespace BigIntNS
{

/**
    possible BigInt exceptions.
    All BigInt exceptions derived from Error class, which derived from
    standard std::excption class.
    This means, that construction
        catch (BigInt::Error) {  }
    will catch all BigInt expecions, and construction
        catch (std::exception) { }
    will catch all standard exceptions, including BigInt exceptions
*/
class Error : public std::exception { }; // base error exception, derived from std::exception
class MemoryError : public Error { }; // memory allocation error

// owerflow when converting BigInt into integer, or when finding 1-bit in the number
class OverflowError : public Error { };

class DivideByZeroError : public Error { }; // dividing by zero
class WrongCharError : public Error { }; // unexpected char when converting number from string

// wrong base passed into function, when converting BigInt number into string
// or wrong base passed in millerTest() function
class WrongBaseError : public Error { };

// throws on jacobi() function, if argument is even.
class EvenNumberError : public Error { };

// cannot find inv(this) (mod m), becasue GCD(this, m) != 1
class InvNumberError : public Error { };

class ShortStringError : public Error { }; // length of string, which must be converted to BigInt, must be greater than 0
class NegativeNumberError : public Error { }; // cannot calculate square root from negative number

} // namespace BigIntNS

#endif // #ifndef BIG_INT_EXCEPTIONS_HPP
