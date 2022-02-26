
#ifndef JENSEN_DIAG_GENFUNC_H
#define JENSEN_DIAG_GENFUNC_H

#include <iostream>
#include "settings.h"

using namespace std;

const int PREENTRY_BITS=7;
const int INDEX_BITS=6;

extern int global_n;
class u128_addable;
class u64_addable_mod;
//typedef u128_addable gf_type;
typedef u64_addable_mod gf_type;

int bit_size_64(unsigned long long);

class PackedGenFunc;

class u128_addable{
    /* A very simple 128 bit integer using two long long which supports only addition.*/
public:
    unsigned long long hi, lo;
    u128_addable();
    u128_addable(unsigned long long y);
    void add(const u128_addable &x);
    u128_addable &operator+=(const u128_addable &y);
    explicit operator bool ();
    int bit_size();
};

ostream& operator<<(ostream& os, const u128_addable& x);

class u64_addable_mod{
public:
    unsigned long long x;
    u64_addable_mod();
    u64_addable_mod(unsigned long long y);
    void add(const u64_addable_mod &x);
    u64_addable_mod &operator+=(const u64_addable_mod &y);
    explicit operator bool ();
    int bit_size();
};
ostream& operator<<(ostream& os, const u64_addable_mod& x);

class GenFunc{
private:
#ifdef GF_USE_PREALLOCATED
    gf_type g_func[GF_CONST_SIZE+1];
#else
    gf_type *g_func;
#endif
#ifdef GF_USE_PACKING
    PackedGenFunc* pgf;
#endif
    int8_t max_n;
    int8_t min_n;
public:
    explicit GenFunc();
    GenFunc(const GenFunc &gf2);
    GenFunc(GenFunc &gf2, int mul);
    ~GenFunc();
    void add(GenFunc &gf2, int mul=0);
    bool is_empty(int mul = 0);
    void pack(bool repack_flag = true ,int preentry_bits = PREENTRY_BITS, int index_bits = INDEX_BITS);
    bool unpack(int preentry_bits = PREENTRY_BITS, int index_bits = INDEX_BITS);
    gf_type at(int i);
    void set_at(int i, gf_type x);
    void clear_from(int i);
    int size();
    bool is_valid();
    GenFunc& operator=(const GenFunc& other);
    friend class PackedGenFunc;
};

class PackedGenFunc{
public:
    /* This is not optimal, memory-wise, however, it allows us to insert the bits in 64 bits blocks and saves
     * us the overflow at the end of the array. Can change to char if memory will be the bottleneck, however
     * this should be negligible.
     * bitset can not be used since it must have precompiled size, and vector<bool> have 40 byte overhead which is too much.*/
    unsigned long long * bit_array;
    PackedGenFunc(GenFunc* gf, int preentry_bits = PREENTRY_BITS, int index_bits = INDEX_BITS);
    PackedGenFunc(const PackedGenFunc& other);
    ~PackedGenFunc();
    void insert(int pos, unsigned long long x, int x_bits);
    unsigned long long get(int pos, int x_bits) const;
    PackedGenFunc& operator=(const PackedGenFunc& other);
};


#endif //JENSEN_DIAG_GENFUNC_H
