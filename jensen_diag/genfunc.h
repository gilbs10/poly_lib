
#ifndef JENSEN_DIAG_GENFUNC_H
#define JENSEN_DIAG_GENFUNC_H

class GenFunc;

#include "settings.h"
#include "PackedArray.h"
#include "SigDict.h"
#include "data_types.h"


using namespace std;

typedef u128_addable gf_type; // Slightly faster than u128_full
//typedef u128_full gf_type;
//typedef u64_addable_mod gf_type;


const int PREENTRY_BITS=7;
const int INDEX_BITS=6; // 6 For n<=63, 7 for higher. I wish 8 was needed.


class GenFunc{
private:
#ifdef GF_USE_PREALLOCATED
    gf_type g_func[GF_CONST_SIZE+1];
#else
    gf_type *g_func;
#endif
#ifdef GF_USE_PACKING
    PackedArray* pgf;
#endif
    int8_t max_n;
    int8_t min_n;
public:
    explicit GenFunc();
    GenFunc(const GenFunc &gf2);
    GenFunc(GenFunc &gf2, int mul);
    GenFunc(PackedArray& pgf, int pos);
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
    int bit_size();
    int is_packed();
    friend class SigDict;
    GenFunc& operator=(const GenFunc& other);
};



#endif //JENSEN_DIAG_GENFUNC_H
