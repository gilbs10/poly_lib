//
// Created by gilbe on 9.3.2022.
//

#include "data_types.h"
#include "settings.h"

int bit_size_64(unsigned long long x){
    if(x){
        return 64 - __builtin_clzll(x);
    }
    return 0;
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
        return bit_size_64(hi)+64;
    }
    return bit_size_64(lo);
}



ostream& operator<<(ostream& os, const u128_addable& x){
    os << "(" << x.hi << ", " << x.lo << ")";
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
