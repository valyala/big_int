#include <iostream>
#include "big_int.hpp"
#include "big_int_iostream.hpp"

using namespace BigIntNS;
using std::string;
using std::cout;
using std::cin;

// tests constructors and destructors of BigInt class
void test_constructors()
{
    int n = 10;
    const int n1 = 20;
    char *c_str = "12345678900987654321";
    const char *c_str1 = "-110101101010110";
    string str = "+123456789097889879";
    const string str1 = "10234023441";

    BigInt a; // default constructor
    BigInt b = n; // construct from [int]
    BigInt c = c_str; // construct from [char *]
    BigInt d = str; // construct from [string]
    
    // construct from [const int] and reserve 20 words of memory
    BigInt e(n1, size_t(20));

    // construct from [const char *] with base = 2
    BigInt f(c_str1, 2);

    // construct from [const char *] with base = 5, reserve 100 words of memory
    BigInt g(str1.c_str(), 5, size_t(100));

    // construct from [const string] with base = 7
    BigInt h(str1, 7);

    // construct from [const string] with base = 5,  reserve 30 words of memory
    BigInt i(str1, 5, 30);

    // copy constructor
    const BigInt j = d;

    // copy constructor from [const BigInt]
    BigInt k = j;

    // create an array of BigInt's
    BigInt arr[10];

    // dynamic allocation
    BigInt *pa = new BigInt; // default constructor
    BigInt *pb = new BigInt(n); // construct from [int]
    BigInt *pc = new BigInt(c_str); // construct from [char *]
    BigInt *pd = new BigInt(str); // construct from [string]
    
    // construct from [const int] and reserve 20 words of memory
    BigInt *pe = new BigInt(n1, size_t(20));

    // construct from [const char *] with base = 2
    BigInt *pf = new BigInt(c_str1, 2);

    // construct from [const char *] with base = 5, reserve 100 words of memory
    BigInt *pg = new BigInt(str1.c_str(), 5, size_t(100));

    // construct from [const string] with base = 7
    BigInt *ph = new BigInt(str1, 7);

    // construct from [const string] with base = 5,  reserve 30 words of memory
    BigInt *pi = new BigInt(str1, 5, 30);

    // copy constructor
    BigInt *pj = new BigInt(d);

    // copy constructor from [const BigInt]
    BigInt *pk = new BigInt(j);

    // create an array of BigInt's
    BigInt *parr = new BigInt[10];

    // call destructors for dinamically allocated objects
    delete[] parr;
    delete pk;
    delete pj;
    delete pi;
    delete ph;
    delete pg;
    delete pf;
    delete pe;
    delete pd;
    delete pc;
    delete pb;
    delete pa;
}

// tests copy operators
void test_copy()
{
    int n = 10;
    const int n1 = 20;
    char *c_str = "-123456789";
    string str = "0988794324234";
    const string str1 = str;

    BigInt a;
    BigInt b = str;
    const BigInt c = n1;

    a = n; // copy from [int]
    a = n1; // copy from [const int]
    a = c_str; // copy from [char *]
    a = str.c_str(); // copy from [const char *]
    a = str; // copy from [string]
    a = str1; // copy from [const string]
    a = a; // copy from self ;)
    a = b; // copy from [BigInt]
    a = c; // copy from [const BigInt]
}

// test service functions
void test_service()
{
    bool flag;

    int n = 10;
    const int n1 = 20;
    char *c_str = "-123456789";
    const char *c_str1;
    string str = "0988794324234";
    const string str1 = str;

    BigInt a = -123;
    BigInt b = str;
    const BigInt c = n1;

    // synonyms for operator = ()
    a.assign(n);
    a.assign(n1);
    a.assign(c_str);
    a.assign(str.c_str());
    a.assign(str);
    a.assign(str1);
    a.assign(a);
    a.assign(b);
    a.assign(c);

    // try to reserve 1M words of memory for [a]
    a.reserve(1 << 20);
    a = 10;
    if (a.length() != 1 || a.size() != 1) {
        // length of [a] must be equal to 1
        cout << "wrong result of length() function\n";
        throw Error();
    }

    if (a.capacity() < (1 << 20)) {
        // number of reserved words must be >= 1M
        cout << "wrong result of capacity() function\n";
        throw Error();
    }

    // assigns n to [a]
    a.fromInt(n);

    // assigns n1 to [a] and allocates 34 words of memory
    a.fromInt(n1, 34);

    // assigns c_str to [a] with base = 16
    a.fromString(c_str, 16);

    // assigns [str] to [a] with base = 30 and allocates 100 words of memory
    a.fromString(str, 30, 100);

    // try to pass low base into fromString()
    flag = false;
    try { a.fromString(str, 1); }
    catch (WrongBaseError) { flag = true; }
    if (!flag) {
        cout << "cannot catch WrongBaseError. 1\n";
        throw Error();
    }

    // try to pass high base into fromString()
    flag = false;
    try { a.fromString(str, 100); }
    catch (WrongBaseError) { flag = true; }
    if (!flag) {
        cout << "cannto catch WrongBaseError. 2\n";
        throw Error();
    }

    // catch WrongCharError
    flag = false;
    try { a.fromString("327dfsf7889", 10); }
    catch (WrongCharError) { flag = true; }
    if (!flag) {
        cout << "cannot catch WrongCharError\n";
        throw Error();
    }

    // catch ShortStringError
    flag = false;
    try { a.fromString("", 10); }
    catch (ShortStringError) { flag = true; }
    if (!flag) {
        cout << "cannot catch ShortStringError\n";
        throw Error();
    }

    // check toInt()
    n = a.fromString("100").toInt();

    // try to convert large number into integer (exception must be thrown)
    flag = false;
    a = "12345678900987654321123456789009876543211";
    try { a.toInt(); }
    catch (OverflowError) { flag = true; }
    if (!flag) {
        cout << "cannot catch OverflowError\n";
        throw Error();
    }

    // clone number. Original number must not be changed
    a = 10;
    a.clone() = 20;
    if (a.toInt() != 10) {
        cout << "wrong result of clone() function\n";
        throw Error();
    }

    // try to serialize / unserialize number without sign
    a = -123456;
    str = a.serialize(); // serialize without sign
    if (a.unserialize(str).toInt() != 123456) {
        cout << "wrong result of serialize() / unserialize() functions (without sign)\n";
        cout << "a = " << a << "\n";
        throw Error();
    }

    // try to serialize / unserialize number with sign
    a = -12345;
    str = a.serialize(true); // serialize with sign
    if (a.unserialize(str, true).toInt() != -12345) {
        cout << "wrong result of serialize() / unserialize() functions (with sign)\n";
        cout << "a = " << a << "\n";
        throw Error();
    }

    c_str1 = a.getVersion();
    c_str1 = a.getBuildDate();

    // try to perform base convertation
    str = BigInt::baseConvert("255", 10, 2);
    if (str != "11111111") {
        cout << "wrong result of baseConvert()\n";
        throw Error();
    }
}


// test assign operators
void test_assign_operators()
{
    BigInt a = 10;
    const BigInt b = -2;

    a += 4;
    if (a.toInt() != 14) {
        cout << "error in += operator. 1\n";
        throw Error();
    }

    a -= "30";
    if (a.toInt() != -16) {
        cout << "error in -= operator\n";
        throw Error();
    }

    a *= string("15");
    if (a.toInt() != -240) {
        cout << "error in *= operator\n";
        throw Error();
    }

    a /= b;
    if (a.toInt() != 120) {
        cout << "error in /= operator\n";
        throw Error();
    }

    a %= 55;
    if (a.toInt() != 10) {
        cout << "error in %= operator\n";
        throw Error();
    }

    a |= 7;
    if (a.toInt() != 15) {
        cout << "error in |= operator\n";
        cout << "a=" << a << "\n";
        throw Error();
    }

    a &= 26;
    if (a.toInt() != 10) {
        cout << "error in &= operator\n";
        throw Error();
    }

    a ^= 27;
    if (a.toInt() != 17) {
        cout << "error in ^= operator\n";
        throw Error();
    }

    a <<= 3;
    if (a.toInt() != 136) {
        cout << "error in <<= operator\n";
        throw Error();
    }

    a >>= 4;
    if (a.toInt() != 8) {
        cout << "error in >>= operator\n";
        throw Error();
    }

}

// tests unary operators
void test_unary_operators()
{
    BigInt a = 10;

    if ((++a).toInt() != 11) {
        cout << "error in prefix ++ operator\n";
        throw Error();
    }

    if ((a--).toInt() != 11 || a.toInt() != 10) {
        cout << "error in postfix -- operator\n";
        throw Error();
    }

    if ((--a).toInt() != 9) {
        cout << "error in prefix -- operator\n";
        throw Error();
    }

    if ((a++).toInt() != 9 || a.toInt() != 10) {
        cout << "error in postfix ++ operator\n";
        throw Error();
    }

    if ((-a).toInt() != -10) {
        cout << "error in unary - operator\n";
        throw Error();
    }
}


// tests binary operators
void test_binary_operators()
{
    BigInt a = 10, b = 20, c = a - b;

    if (c.toInt() != -10) {
        cout << "error in type casting from BinOperands into BigInt\n";
        throw Error();
    }

    // test comparision operators
    if (a != string("10") || "20" != b || -10 != c) {
        cout << "error in != operator\n";
        throw Error();
    }

    if (a == string("20") || "10" == b || 10 == c) {
        cout << "error in == operator\n";
        throw Error();
    }

    // automatic conversion into BigInt
    if (string("20") < 10 || 20 < BigInt("10")) {
        cout << "error in < operator\n";
        throw Error();
    }

    // check for bogus operator overloading
    if (string("20") + "23" != string("2023") || 3 + 4 != 7
     || 78 + string("234") != 312 || string("23") * 56 != "1288"
     || string("77") - "1" != 76) {
        cout << "bogus operator redefintion\n";
        throw Error();
    }

    if ("78" <= string("77") || 97 >= string("98")
     || !(8 <= BigInt(8)) || !("980" >= BigInt("980"))) {
        cout << "error in >= or <= operators\n";
        throw Error();
    }

    // test arithmetics operators
    a = 10 + (BigInt("20") + 30) + "50";
    if (a != 110) {
        cout << "error in BinOperands calculations and + operator\n";
        throw Error();
    }

    a = 2379 / ((10 - BigInt("20") * 30 + 34) % 31) - 18;
    if (a != -100) {
        cout << "error in BinOperands calculations and -, *, / and % operators\n";
        throw Error();
    }

    a = 1000 - (456 ^ ((b.fromInt(1234) | c.fromString("123456")) & 7896));
    if (a != -816) {
        cout << "error in BinOperands calculations and |, &, ^ operators\n";
        throw Error();
    }

    b = ((BigInt("1234") >> -3) * 43) << 4;
    if (b != "6791936") {
        cout << "error in BinOperands calculations and >>, << operators\n";
        throw Error();
    }
}

// test bitset functions
void test_bitset_functions()
{
    BigInt a = 12345;
    BigInt b = 43218;

    if (a.bitOr(43219) != 47355 || a.bitAnd(8031) != 6235
     || a.bitAndNot(4567) != 2056 || a.bitXor(7325) != 5269) {
        cout << "error in bitset functions with start_pos = 0\n";
        throw Error();
    }

    if (a.clone().bitOr(b, 5) != a.clone().bitOr(b << 5)
     || a.clone().bitXor(b, 100) != a.clone().bitXor(b << 100)
     || a.clone().bitAnd(b, 7) != a.clone().bitAnd((b << 7) | BigInt().subInt(0, 7, true))) {
        cout << "error in bitset functions with start_pos != 0\n";
        throw Error();
    }

    if (a.fromInt(1234).lShift(5) != 39488 || a.rShift(10) != 38) {
        cout << "error in lShift() / rShift() functions\n";
        throw Error();
    }

    if (a.fromString("100101011101110111010101111010100010111110111", 2).bitLength() != 45) {
        cout << "error in bitLength() function\n";
        cout << a.bitLength() << "\n";
        throw Error();
    }

    if (a.bit1Cnt() != 29) {
        cout << "error in bit1Cnt() function\n";
        throw Error();
    }

    if (a.subInt(9, 10).toString(2) != "1010100010") {
        cout << "error in subInt() function\n";
        throw Error();
    }


}

int main()
{
    // test constructors and destructors
    try { test_constructors(); }
    catch (Error) {
        cout << "error in test_constructors()\n";
    }

    // test copy operators
    try { test_copy(); }
    catch (Error) {
        cout << "error in test_copy()\n";
    }

    // test service functions
    try { test_service(); }
    catch (Error) {
        cout << "error in test_service()\n";
    }

    // test overriden operators
    try { test_assign_operators(); }
    catch (Error) {
        cout << "error in test_assign_operators()\n";
    }

    // test unary operators
    try { test_unary_operators(); }
    catch (Error) {
        cout << "error in test_unary_operators()\n";
    }

    // test binary operators
    try { test_binary_operators(); }
    catch (Error) {
        cout << "error in test_binary_operators()\n";
    }

    try { test_bitset_functions(); }
    catch (Error) {
        cout << "error in test_bitset_functions()\n";
    }

    BigInt a = 10;
    BigInt b = "1234567798798989898980098768768768";
    BigInt c;
    const BigInt d = 98;

    a.reserve(1 << 21);
    a = 123;
    a = "1234";
    b = a;
    c = string("2342");
    a = string("987") + (d + 1) + "56789";
    b = a ^ ("78" | b) & 9;
    c = ((d << 2) + 1 ) >> -3;
    c = -56;
    c.fromInt(-43).clone().abs();
    b += 10;
    a.cmpMod(b, c);
    a.sqrMod(b);
    a.addMod(c, b);
    cout << "b=" << b << ", c=" << c.toString(2) << "\n";

    a += -(10 + b);
    a += "20";
    a += a;
    a -= b;
    a -= string("23454633487787");
    a *= 2;
    a /= 3;
    a.mod("45");
    c = 23;
    cout << "a=" << (a + 10) << ", b=" << b << ", c=" << c++ << "\n";
    a = b;
    b = "1234";
    c = 456;
    a = a;
    cout << "enter c=";
    cin >> c;
    cout << "a=" << a << ", b=" << b << ", c=" << ++c << "\n";
    cout << BigInt::baseConvert("1111", 2) << "\n";

    if (c == "10") cout << "c = 10\n";
    else if (10 < c) cout << "c > 10\n";
    else cout << "c < 10\n";

    cout << "serializing a...";
    string s = a.serialize();
    cout << "end" << std::endl;

    cout << "unserializing a to c...";
    c.unserialize(s);
    cout << "end" << std::endl;

    a = -1234;
    cout <<"abs(a)=" << a.clone().abs() << ", a=" << a << ", c=" << c << "\n";

    cout << a.fromInt(10).addMul(10, "20") << "\n";

    b = 20;
    a = "11" + (-b + 90) / 10;
    cout << "a=" << a << ", b=" << b << "\n";

    a = 10;
    b = a.divExt(3, c);
    cout << "q = " << a << ", r=" << c << "\n";

    cout << (a |= 8);
    cout << ", lshift = " << (a <<= 10);
    cout << ", rshift=" << (a >>=11) << "\n";
    a = a | 10;
    b = (10 | (b ^ "143") & 0xfff) << 10;
    a = b >> 2;
    b = -(0 + a);
    if (b == b + 0) cout << "b = b\n";
    else cout << "b != b\n";
    cout << "a=" << a << ", b=" << b.rand(100).GCD(345) << "\n";
    cout << "result=" << a.clone().GCDExt(10, c, b) << "\n";
    cout << c << "*" << a << "+" << b << "*10=result\n";
    cout << a.pow(5) << "\n";
    cout << "miller test=" << a.millerTest(3) << "\n";
    cout << "jacobi=" << a.jacobi(3) << "\n";
}
