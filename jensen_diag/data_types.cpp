//
// Created by gilbe on 9.3.2022.
//

#include "data_types.h"
#include "settings.h"

unsigned long long  bit_size_n(unsigned long long x){
    if(x){
        return 64 - __builtin_clzll(x);
    }
    return 0;
}

unsigned long long  bit_size_n(__int128 x){
    if(get_hi(x)){
        return 64 + bit_size_n(get_hi(x));
    }
    return bit_size_n(get_lo(x));
}

unsigned long long get_hi(__int128 x){
    return x>>64;
}

unsigned long long get_lo(__int128 x){
    return (x<<64)>>64;
}

u128_addable::u128_addable() : hi(0), lo(0){
}
u128_addable::u128_addable(unsigned long long y): hi(0), lo(y){
}
void u128_addable::add(const u128_addable &x){
    // Assuming no overflow from 128 bit
    hi += x.hi;
    hi += __builtin_add_overflow(lo,x.lo,&lo);
}
u128_addable &u128_addable::operator+=(const u128_addable &y) {
    this->add(y);
    return *this;
}
u128_addable::operator bool(){
    return hi!=0 or lo!=0;
}

int u128_addable::bit_size(){
    if(hi){
        return bit_size_n(hi) + 64;
    }
    return bit_size_n(lo);
}

ostream& operator<<(ostream& os, const u128_addable& x){
    os << "(" << x.hi << ", " << x.lo << ")";
    return os;
}

u192_addable::u192_addable() : hi(0), lo(0) {}
u192_addable::u192_addable(unsigned long long y) : hi(0), lo(y) {}
void u192_addable::add(const u192_addable &x) {
    hi += __builtin_add_overflow(lo,x.lo,&lo);
    hi += x.hi;
}
u192_addable &u192_addable::operator+=(const u192_addable &y) {
    this->add(y);
    return *this;
}
u192_addable::operator bool() {
    return lo || hi;
}
int u192_addable::bit_size() {
    if(hi){
        return hi.bit_size() + 64;
    }
    return bit_size_n(lo);
}
ostream& operator<<(ostream& os, const u192_addable& x){
    os << "(" << x.hi.hi << ", " << x.hi.lo << ", " << x.lo << ")";
    return os;
}

u128_full::u128_full() : x(0){
}
u128_full::u128_full(unsigned long long y) : x(y){
}
void u128_full::add(const u128_full &other){
    x += other.x;
}
u128_full &u128_full::operator+=(const u128_full &other){
    this->add(other);
    return *this;
}
u128_full::operator bool (){
    return x!=0;
}

int u128_full::bit_size(){
    return 128 - __builtin_clzll(x);
}
ostream& operator<<(ostream& os, const u128_full& x){
    __int128 d = 1;
    d << 64;
    os << "(" << (unsigned long long)(x.x / d) << ", " <<  (unsigned long long)(x.x % d) << ")";
    return os;
}

#ifdef GF_MOD_CONST
u64_addable_mod::u64_addable_mod() : x(GF_MOD_CONST){
}
u64_addable_mod::u64_addable_mod(unsigned long long y) : x(y % GF_MOD_CONST){
    if(x == 0){
        x = GF_MOD_CONST;
    }
}
void u64_addable_mod::add(const u64_addable_mod &y){
    x += y.x;
    x %= GF_MOD_CONST;
    if(x == 0){
        x = GF_MOD_CONST;
    }
}
u64_addable_mod& u64_addable_mod::operator+=(const u64_addable_mod &y){
    x += y.x;
    x %= GF_MOD_CONST;
    if(x == 0){
        x = GF_MOD_CONST;
    }
    return *this;
}
u64_addable_mod::operator bool (){
    return x!=0;
}
int u64_addable_mod::bit_size(){
    return bit_size_64(x);
}

ostream& operator<<(ostream& os, const u64_addable_mod& x){
    os << x.x;
    return os;
}

#endif