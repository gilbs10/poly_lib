#include <iostream>
#include "genfunc.h"

using namespace std;

inline int8_t min(int8_t x, int8_t y){
    return x < y ? x : y;
}
inline int8_t max(int8_t x, int8_t y){
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



GenFunc::GenFunc(){
    #ifndef GF_USE_PREALLOCATED
        g_func = new gf_type[global_n+1];
    #endif
    max_n = 0; // Pos of the element *after* the last non-zero element
    min_n = global_n+2;
    #ifdef GF_USE_PACKING
    pgf = nullptr;
    pack();
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
            pgf = new PackedArray(*gf2.pgf);
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
        pgf = new PackedArray(*gf2.pgf);
    } else {
        pgf = nullptr;
    }
#endif
}

GenFunc::GenFunc(PackedArray &new_pgf, int pos) {
    int start_pos = pos;
    g_func = new gf_type[global_n+1]();
    pgf = nullptr;
    max_n = 0;
    min_n = global_n+1;
    int entries, index, x_bits;
    pos += new_pgf.fetch(pos, &entries, INDEX_BITS);
    for (int i = 0; i < entries; ++i) {
        pos += new_pgf.fetch(pos, &index, INDEX_BITS);
        pos += new_pgf.fetch(pos, &x_bits, PREENTRY_BITS);
        pos += new_pgf.fetch(pos, g_func+index, x_bits);
        max_n = max(max_n, index+1);
        min_n = min(min_n, index);
    }
    pack();
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

void GenFunc::pack(bool repack_flag, unsigned long long  preentry_bits, unsigned long long  index_bits){
#ifdef GF_USE_PACKING
    if(!repack_flag || pgf){
        return;
    }
    unsigned long long  bit_sum = index_bits;
    unsigned long long  element_bits[size()+1];
    unsigned long long c = 0;
    for (int i = min_n; i < max_n; ++i)  {
        if(g_func[i]){
            element_bits[i] = g_func[i].bit_size();
            bit_sum += element_bits[i] + preentry_bits + index_bits;
            c++;
        } else {
            element_bits[i] = 0;
        }
    }
    pgf = new PackedArray(bit_sum);
    int pos = 0;
    pos += pgf->insert(pos, c, index_bits);
    for (int i = min_n; i < max_n; ++i) {
        if(g_func[i]){
            pos += pgf->insert(pos, (unsigned long long)i, index_bits);
            pos += pgf->insert(pos, element_bits[i], preentry_bits);
            pos += pgf->insert(pos, g_func[i], element_bits[i]);
        }
    }
    delete[](this->g_func);
    g_func = nullptr;
#endif
}


bool GenFunc::unpack(int preentry_bits, int index_bits){
#ifdef GF_USE_PACKING
    if(!pgf){ // Nothing to do
        return false;
    }
    g_func = new gf_type[global_n+1]();
    max_n = 0;
    min_n = global_n+1;
    int entries, index, x_bits;
    int pos = 0;
    pos = pgf->fetch(pos, &entries, index_bits);
    for (int i = 0; i < entries; ++i) {
        pos += pgf->fetch(pos, &index, index_bits);
        pos += pgf->fetch(pos, &x_bits, preentry_bits);
        pos += pgf->fetch(pos, g_func+index, x_bits);
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
        pgf = new PackedArray(*gf2.pgf);
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

int GenFunc::bit_size() {
    return pgf->bit_sum;
}
int GenFunc::is_packed() {
    return pgf && !g_func;
}


