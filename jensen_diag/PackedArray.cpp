//
// Created by gilbe on 9.3.2022.
//

#include "PackedArray.h"

PackedArray::PackedArray() : bit_sum(0), bit_array(nullptr) {
}

PackedArray::PackedArray(unsigned long long bit_sum) : bit_sum(bit_sum) {
    bit_array = new unsigned long long[(bit_sum-1) / 64 + 1]();
}

int PackedArray::insert(int pos, unsigned long long x, int x_bits){
    int slack = 64 - (pos % 64);
    int i = pos / 64;
    if(x_bits > slack){
        bit_array[i] |= x>>(x_bits-slack);
        bit_array[i+1] |= x<<(64-(x_bits-slack));
    } else {
        bit_array[i] |= x<<(slack-x_bits);
    }
    return x_bits;
}

unsigned long long PackedArray::get(int pos, int x_bits) const {
    int slack = 64 - (pos % 64);
    int i = pos / 64;
    unsigned long long res = 0;
    if(x_bits > slack){
        res = (bit_array[i]<<(64-slack))>>(64-x_bits);
        res |= bit_array[i+1]>>(64-(x_bits-slack));

    } else {
        res |= (bit_array[i]<<(64-slack))>>(64-x_bits);
    }
    return res;
}


int PackedArray::insert(int pos, u128_addable x, int x_bits){
    if(x.hi){
        insert(pos, x.hi, x_bits-64);
        pos += x_bits - 64;
        x_bits = 64;
    }
    insert(pos, x.lo, x_bits);
    return x_bits;
}

int PackedArray::insert(int pos, u64_addable_mod x, int x_bits){
    insert(pos, x.x, x_bits);
    return x_bits;
}

int PackedArray::insert(int pos, u128_full x, int x_bits){
    insert(pos, x.x, x_bits);
    return x_bits;
}

int PackedArray::insert(int pos, PackedArray& pa, int x_bits){
    for (int i = 0; i < pa.array_length() - 1; ++i) {
        pos += insert(pos, pa.bit_array[i], 64);
    }
    int bits = x_bits%64;
    if(!bits){
        bits = 64;
    }
    unsigned long long last = pa.bit_array[pa.array_length()-1];
    last >>= 64-bits;
    insert(pos, last, bits);

    return x_bits;
}



int PackedArray::fetch(int pos, u128_addable* x ,int x_bits) const{
    x->hi = 0;
    if(x_bits > 64){
        x->hi = get(pos, x_bits-64);
        pos += x_bits-64;
        x_bits=64;
    }
    x->lo = get(pos, x_bits);
    return x_bits;
}

int PackedArray::fetch(int pos, int* x, int x_bits) const{
    *x = get(pos, x_bits);
    return x_bits;
}

int PackedArray::fetch(int pos, unsigned long long* x, int x_bits) const{
    *x = get(pos, x_bits);
    return x_bits;
}

int PackedArray::fetch(int pos, u64_addable_mod* x, int x_bits) const{
    x->x = get(pos, x_bits);
    return x_bits;
}

int PackedArray::fetch(int pos, u128_full* x, int x_bits) const{
    x->x = get(pos, x_bits);
    return x_bits;
}

int PackedArray::array_length() {
    return (bit_sum-1) / 64 + 1;
}


PackedArray::PackedArray(const PackedArray &other) {
    bit_sum = other.bit_sum;
    bit_array = new unsigned long long[array_length()];
    for (int i = 0; i < array_length(); ++i) {
        bit_array[i] = other.bit_array[i];
    }
}

PackedArray::~PackedArray(){
    delete[] bit_array;
}

PackedArray &PackedArray::operator=(const PackedArray &other) {
    bit_sum = other.bit_sum;
    bit_array = new unsigned long long[array_length()];
    for (int i = 0; i < array_length(); ++i) {
        bit_array[i] = other.bit_array[i];
    }
    return *this;
}
