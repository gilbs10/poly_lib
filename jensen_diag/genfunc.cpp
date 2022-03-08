#include <iostream>
#include "genfunc.h"

using namespace std;

int8_t min(int8_t x, int8_t y){
    return x < y ? x : y;
}
int8_t max(int8_t x, int8_t y){
    return x > y ? x : y;
}

void print_bit_array(unsigned long long* bit_array){
    for (int i = 0; i < 2; ++i) {
        unsigned long long ba = bit_array[i];
        unsigned long long mask = 1;
        mask <<= 63;
        for (int j = 0; j < 64; ++j) {
            if(j%8 == 0){
                cout << " ";
            }
            cout << bool(ba & mask);
            mask >>= 1;
        }
        cout << endl;
    }
    cout << endl;
}


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

u64_addable_mod::u64_addable_mod() : x(0){
}
u64_addable_mod::u64_addable_mod(unsigned long long y) : x(y){
}
void u64_addable_mod::add(const u64_addable_mod &y){
    x += y.x;
    x %= GF_MOD_CONST;
}
u64_addable_mod& u64_addable_mod::operator+=(const u64_addable_mod &y){
    x += y.x;
    x %= GF_MOD_CONST;
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

GenFunc::GenFunc(){
    #ifndef GF_USE_PREALLOCATED
        g_func = new gf_type[global_n+1];
    #endif
//    for (int i = 0; i <= global_n; ++i) {
//        g_func[i] = 0;
//    };
    max_n = 0; // Pos of the element *after* the last non-zero element
    min_n = global_n+2;
    #ifdef GF_USE_PACKING
    pgf = new PackedGenFunc(this);
    delete[](this->g_func);
    g_func = nullptr;
    #endif

}

GenFunc::GenFunc(GenFunc &gf2, int mul){
    int target_n = min(global_n - mul + 1, gf2.max_n);
#ifdef GF_USE_PACKING
    bool packed_flag = false;
    if(mul){
        packed_flag = gf2.unpack();
    }
#endif
    if (gf2.g_func) {
        #ifndef GF_USE_PREALLOCATED
        g_func = new gf_type[global_n + 1];
        #endif
        for (int i = gf2.min_n; i < target_n; ++i) {
            g_func[i + mul] = gf2.g_func[i];
        }
    }
#ifndef GF_USE_PREALLOCATED
    else{
        g_func = nullptr;
    }
#endif
    max_n = target_n+mul;
    min_n = gf2.min_n+mul;
    #ifdef GF_USE_PACKING
        if(gf2.pgf){
            pgf = new PackedGenFunc(*gf2.pgf);
        } else {
            pgf = nullptr;
        }
        gf2.pack(packed_flag);
    #endif
}


GenFunc::GenFunc(const GenFunc &gf2){
    int target_n = min(global_n + 1, gf2.max_n);
    if (gf2.g_func) {
#ifndef GF_USE_PREALLOCATED
        g_func = new gf_type[global_n + 1];
#endif
        for (int i = gf2.min_n; i < target_n; ++i) {
            g_func[i] = gf2.g_func[i];
        }
    }
#ifndef GF_USE_PREALLOCATED
    else{
        g_func = nullptr;
    }
#endif
    max_n = target_n;
    min_n = gf2.min_n;
#ifdef GF_USE_PACKING
    if(gf2.pgf){
        pgf = new PackedGenFunc(*gf2.pgf);
    } else {
        pgf = nullptr;
    }
#endif
}



GenFunc::~GenFunc(){
#ifndef GF_USE_PREALLOCATED
    delete[](g_func);
#endif
#ifdef GF_USE_PACKING
    delete(pgf);
#endif
}
void GenFunc::add(GenFunc &gf2, int mul){
    // Assuming both functions are of size n
    #ifdef GF_USE_PACKING
    bool packed_flag1 = unpack();
    bool packed_flag2 = gf2.unpack();
    #endif
    int target_n = min(global_n - mul + 1, gf2.max_n);
    for (int i = gf2.min_n; i < target_n; ++i) {
        g_func[i+mul] += gf2.g_func[i];
    }
    max_n = max(max_n, target_n+mul);
    min_n = min(min_n, gf2.min_n+mul);
    #ifdef GF_USE_PACKING
    pack(packed_flag1);
    gf2.pack(packed_flag2);
    #endif
}
bool GenFunc::is_empty(int mul){
    return max_n <= min_n;
}

void GenFunc::pack(bool repack_flag, int preentry_bits, int index_bits){
#ifdef GF_USE_PACKING
    if(!repack_flag || pgf){
        return;
    }
    pgf = new PackedGenFunc(this, preentry_bits, index_bits);
    delete[](this->g_func);
    g_func = nullptr;
#endif
}


bool GenFunc::unpack(int preentry_bits, int index_bits){
#ifdef GF_USE_PACKING
    if(!pgf){ // Nothing to do
        return false;
    }
    g_func = new gf_type[global_n+1];
    for (int i = 0; i < global_n+1; ++i) {
        g_func[i] = 0;
    }
    max_n = 0;
    min_n = global_n+1;
    int pos = index_bits; // Array len is unnecessary
    int entries = pgf->get(pos, index_bits);
    pos += index_bits;
    for (int i = 0; i < entries; ++i) {
        int index = pgf->get(pos, index_bits);
        pos += index_bits;
        int x_bits = pgf->get(pos, preentry_bits);
        pos += preentry_bits;
        pgf->fetch(pos, g_func+index, x_bits);
        pos += x_bits;
        max_n = max(max_n, index+1);
        min_n = min(min_n, index);
    }
    delete(pgf);
    pgf = nullptr;
#endif
    return true;
}

GenFunc& GenFunc::operator=(const GenFunc& gf2)
{
    int target_n = min(global_n + 1, gf2.max_n);

    if (gf2.g_func) {
#ifndef GF_USE_PREALLOCATED
        g_func = new gf_type[global_n + 1];
#endif
        for (int i = gf2.min_n; i < target_n; ++i) {
            g_func[i] = gf2.g_func[i];
        }
    }
#ifndef GF_USE_PREALLOCATED
    else{
        g_func = nullptr;
    }
#endif
    max_n = target_n;
    min_n = gf2.min_n;
#ifdef GF_USE_PACKING
    if(gf2.pgf){
        pgf = new PackedGenFunc(*gf2.pgf);
    } else {
        pgf = nullptr;
    }
#endif
    return *this;
}

gf_type GenFunc::at(int i) {
#ifdef GF_USE_PACKING
    bool packed_flag = unpack();
#endif
    gf_type res= g_func[i];
#ifdef GF_USE_PACKING
    pack(packed_flag);
#endif
    return res;
}

void GenFunc::set_at(int i, gf_type x) {
#ifdef GF_USE_PACKING
    bool packed_flag = unpack();
#endif
    g_func[i] = x;
    if(x){
        min_n = min(min_n, i);
        max_n = max(max_n, i+1);
    }else{
        if(min_n == i){
            min_n++;
        }
        if(max_n-1 == i){
            max_n--;
        }
    }
#ifdef GF_USE_PACKING
    pack(packed_flag);
#endif
}

void GenFunc::clear_from(int i) {
    max_n = min(max_n, i);
}

int GenFunc::size() {
    return global_n;
}

PackedGenFunc::PackedGenFunc(GenFunc* gf, int preentry_bits, int index_bits){
    int bit_sum = 2*index_bits;
    int element_bits[gf->size()+1];
    int c = 0;
    for (int i = gf->min_n; i < gf->max_n; ++i)  {
        if(gf->g_func[i]){
            element_bits[i] = gf->g_func[i].bit_size();
            bit_sum += element_bits[i] + preentry_bits + index_bits;
            c++;
        } else {
            element_bits[i] = 0;
        }
    }
    int array_length = (bit_sum-1) / 64 + 1;
    bit_array = new unsigned long long[array_length];
    for (int i = 0; i < array_length; ++i) {
        bit_array[i] = 0;
    }
    int pos = 0;
    insert(pos, array_length, index_bits);
    pos += index_bits;
    insert(pos, c, index_bits);
    pos += index_bits;
    for (int i = gf->min_n; i < gf->max_n; ++i) {
        if(gf->g_func[i]){
            insert(pos, i, index_bits);
            pos += index_bits;
            insert(pos, element_bits[i], preentry_bits);
            pos += preentry_bits;
            insert(pos, gf->g_func[i], element_bits[i]);
            pos += element_bits[i];
        }
    }
}

PackedGenFunc::PackedGenFunc(const PackedGenFunc &other) {
    int array_length = other.get(0, INDEX_BITS);
    bit_array = new unsigned long long[array_length];
    for (int i = 0; i < array_length; ++i) {
        bit_array[i] = other.bit_array[i];
    }
}

PackedGenFunc::~PackedGenFunc(){
    delete[] bit_array;
}

void PackedGenFunc::insert(int pos, unsigned long long x, int x_bits){
    int slack = 64 - (pos % 64);
    int i = pos / 64;
    if(x_bits > slack){
        bit_array[i] |= x>>(x_bits-slack);
        bit_array[i+1] |= x<<(64-(x_bits-slack));
    } else {
        bit_array[i] |= x<<(slack-x_bits);
    }
}

void PackedGenFunc::insert(int pos, u128_addable x, int x_bits){
    if(x.hi){
        insert(pos, x.hi, x_bits-64);
        pos += x_bits - 64;
        x_bits = 64;
    }
    insert(pos, x.lo, x_bits);
}

void PackedGenFunc::insert(int pos, u64_addable_mod x, int x_bits){
    insert(pos, x.x, x_bits);
}
void PackedGenFunc::insert(int pos, u128_full x, int x_bits){
    insert(pos, x.x, x_bits);
}

unsigned long long PackedGenFunc::get(int pos, int x_bits) const {
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


void PackedGenFunc::fetch(int pos, u128_addable* x ,int x_bits) const{
    x->hi = 0;
    if(x_bits > 64){
        x->hi = get(pos, x_bits-64);
        pos += x_bits-64;
        x_bits=64;
    }
    x->lo = get(pos, x_bits);
}
void PackedGenFunc::fetch(int pos, u64_addable_mod* x, int x_bits) const{
    x->x = get(pos, x_bits);
}

void PackedGenFunc::fetch(int pos, u128_full* x, int x_bits) const{
    x->x = get(pos, x_bits);
}

PackedGenFunc &PackedGenFunc::operator=(const PackedGenFunc &other) {
    int array_length = other.get(0, INDEX_BITS);
    bit_array = new unsigned long long[array_length];
    for (int i = 0; i < array_length; ++i) {
        bit_array[i] = other.bit_array[i];
    }
    return *this;
}
