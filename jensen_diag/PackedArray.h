//
// Created by gilbe on 9.3.2022.
//

#ifndef JENSEN_DIAG_PACKEDARRAY_H
#define JENSEN_DIAG_PACKEDARRAY_H

#include "data_types.h"
#include <fstream>
#include <filesystem>
#include "utils.h"
#include "settings.h"

using namespace std;

class PackedArray {
public:
    /* We use an array since a bitset can not be used since it must have precompiled size,
     * and vector<bool> have 40 byte overhead which is too much.*/
    unsigned long long bit_sum;
    unsigned long long * bit_array;
    PackedArray();
    PackedArray(unsigned long long bitsize);
    PackedArray(const PackedArray &other);
    ~PackedArray();
    virtual int insert(unsigned long long pos, unsigned long long x, int x_bits);
    int insert(unsigned long long pos, u128_addable x, int x_bits);
    int insert(unsigned long long pos, u192_addable x, int x_bits);
    int insert(unsigned long long pos, u64_addable_mod x, int x_bits);
    int insert(unsigned long long pos, u128_full x, int x_bits);
    int insert(unsigned long long pos, __int128 x, int x_bits);
    int insert(unsigned long long pos, PackedArray& x, int x_bits);
    int fetch(unsigned long long pos, int* x ,int x_bits) const;
    int fetch(unsigned long long pos, unsigned long long* x ,int x_bits) const;
    int fetch(unsigned long long pos, u128_addable* x ,int x_bits) const;
    int fetch(unsigned long long pos, u192_addable* x ,int x_bits) const;
    int fetch(unsigned long long pos, u64_addable_mod* x, int x_bits) const;
    int fetch(unsigned long long pos, u128_full* x, int x_bits) const;
    int fetch(unsigned long long pos, __int128* x, int x_bits) const;
    unsigned long long get(unsigned long long pos, int x_bits) const;
    unsigned long long array_length();
    PackedArray &operator=(const PackedArray &other);
};

class PackedArraySwappable : public PackedArray {
public:
    unsigned long long bit_buffer[PAS_BUFFER_SIZE];
    int buffer_pos;
    fstream bit_file;
    filesystem::path bit_file_name;
    bool persistant;
    PackedArraySwappable(unsigned long long bitsize, bool persistant = false);
    PackedArraySwappable(filesystem::path file_name, bool persistant = false);
    ~PackedArraySwappable();
    void create_file();
    void delete_file();
    void open_file();
    void close_file();
    void swap_all();
    void swap_buffer();
    void unswap();
    int buffer_length();
    int insert(unsigned long long pos, unsigned long long x, int x_bits) override;
};


#endif //JENSEN_DIAG_PACKEDARRAY_H
