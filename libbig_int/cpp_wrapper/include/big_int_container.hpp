#ifndef BIG_INT_CONTAINER_HPP
#define BIG_INT_CONTAINER_HPP
#include "big_int_consts.hpp"

namespace BigIntNS
{

// container for numbers
class BigIntContainer
{
    unsigned int *refcnt;
    BigIntAPI::big_int *num;

    void _unlink() {
        if (--*refcnt) return;
        BigIntAPI::big_int_destroy(num);
        delete refcnt;
    }

public:
    // copy constructor
    explicit BigIntContainer(const BigIntContainer &n) : refcnt(n.refcnt), num(n.num)
    {
        ++*refcnt;
    }

    // allocates prealloc_len words for number. Initializes number by 0
    explicit BigIntContainer(size_t prealloc_len) : refcnt(new unsigned  int(1)), num(BigIntAPI::big_int_create(prealloc_len))
    {
        if (!num) {
            delete refcnt;
            throw MemoryError();
        }
    }

    ~BigIntContainer() { _unlink(); }

    // creates new copy of container with refcount = 1;
    BigIntAPI::big_int * clone()
    {
        BigIntAPI::big_int *old = num;
        create(num->len);
        if (BigIntAPI::big_int_copy(old, num)) throw MemoryError();
        return num;
    }

    // creates new container with refcount = 1
    BigIntAPI::big_int * create(size_t prealloc_len = default_prealloc_len)
    {
        if (*refcnt == 1) {
            // there is no need to create additional container
            // try to reserve memory for number [num]
            reserve(prealloc_len);
            return num;
        }
        // try to create additional container
        BigIntContainer tmp(prealloc_len);
        _unlink();
        *this = tmp;
        ++*refcnt;
        return num;
    }

    // assigns container [n] to current container
    void assign(const BigIntContainer &n)
    {
        if (num == n.num) return;
        if (*refcnt > 1) {
            _unlink();
            *this = n;
            ++*refcnt;
        } else {
            if (BigIntAPI::big_int_copy(n.num, num)) throw MemoryError();
        }
    }

    // tryes to reserve [prealloc_len] words for number [num]
    void reserve(size_t prealloc_len)
    {
        if (BigIntAPI::big_int_realloc(num, prealloc_len)) throw MemoryError();
    }

    operator const BigIntAPI::big_int * () const { return num; }
    const BigIntAPI::big_int * operator -> () const { return num; }

    // this class uses default assign operator, which makes bit-to-bit copy of original object

}; // class BigIntContainer

} // namespace BigIntNS

#endif // ifndef BIG_INT_CONTAINER_HPP
