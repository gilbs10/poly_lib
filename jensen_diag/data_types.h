//
// Created by gilbe on 9.3.2022.
//

#ifndef JENSEN_DIAG_DATA_TYPES_H
#define JENSEN_DIAG_DATA_TYPES_H

#include <iostream>

using namespace std;

extern int global_n;
class u128_addable;
class u128_full;
class u64_addable_mod;

int bit_size_64(unsigned long long);

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

class u128_full{
    /* A very simple 128 bit integer using two long long which supports only addition.*/
public:
    __int128 x;
    u128_full();
    u128_full(unsigned long long y);
    void add(const u128_full &x);
    u128_full &operator+=(const u128_full &y);
    explicit operator bool ();
    int bit_size();
};

ostream& operator<<(ostream& os, const u128_full& x);

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


#endif //JENSEN_DIAG_DATA_TYPES_H
