//
// Created by gilbe on 9.3.2022.
//

#ifndef JENSEN_DIAG_PACKEDARRAY_H
#define JENSEN_DIAG_PACKEDARRAY_H

#include "data_types.h"

class PackedArray {
public:
    /* We use an array since a bitset can not be used since it must have precompiled size,
     * and vector<bool> have 40 byte overhead which is too much.*/
    int bit_sum;
    unsigned long long * bit_array;
    PackedArray();
    PackedArray(unsigned long long bitsize);
    PackedArray(const PackedArray &other);
    ~PackedArray();
    int insert(int pos, unsigned long long x, int x_bits);
    int insert(int pos, u128_addable x, int x_bits);
    int insert(int pos, u64_addable_mod x, int x_bits);
    int insert(int pos, u128_full x, int x_bits);
    int insert(int pos, PackedArray& x, int x_bits);
    int fetch(int pos, int* x ,int x_bits) const;
    int fetch(int pos, unsigned long long* x ,int x_bits) const;
    int fetch(int pos, u128_addable* x ,int x_bits) const;
    int fetch(int pos, u64_addable_mod* x, int x_bits) const;
    int fetch(int pos, u128_full* x, int x_bits) const;
    unsigned long long get(int pos, int x_bits) const;
    int array_length();
    PackedArray &operator=(const PackedArray &other);
};




#endif //JENSEN_DIAG_PACKEDARRAY_H