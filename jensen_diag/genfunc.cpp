#include <iostream>
#include "genfunc.h"

using namespace std;


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
GenFunc::GenFunc(int n){
    this->n = n;
    for (int i = 0; i < n; ++i) {
        g_func[i] = 0;
    };
    pgf = nullptr;
    max_n = 0; // Pos of the element *after* the last non-zero element
    min_n = 0;
}

GenFunc::GenFunc(){
    this->n = n;
    pgf = nullptr;
    max_n = 0;
    min_n = 0;
}

GenFunc::GenFunc(GenFunc &gf2, int mul){
    n = gf2.n;
    int target_n = min(n - mul + 1, gf2.max_n);
    for (int i = gf2.min_n; i < target_n; ++i) {
        g_func[i+mul] = gf2.g_func[i];
    }
    pgf = nullptr;
    max_n = target_n+mul;
    min_n = gf2.min_n+mul;
}


GenFunc::GenFunc(const GenFunc &gf2){
    n = gf2.n;
    int target_n = min(n + 1, gf2.max_n);
    for (int i = gf2.min_n; i < target_n; ++i) {
        g_func[i] = gf2.g_func[i];
    }
    pgf = nullptr;
    max_n = target_n;
    min_n = gf2.min_n;
}

GenFunc::~GenFunc(){
    if(pgf){
        delete(pgf);
    }
}
void GenFunc::add(GenFunc &gf2, int mul){
    // Assuming both functions are of size n
    bool packed_flag = false;
    if (pgf){
        unpack();
        packed_flag = true;
    }
    int target_n = min(n - mul + 1, gf2.max_n);
    for (int i = gf2.min_n; i < target_n; ++i) {
        g_func[i+mul] += gf2.g_func[i];
    }
    max_n = max(max_n, target_n+mul);
    min_n = min(min_n, gf2.min_n+mul);
    if(packed_flag){
        pack();
    }
}
bool GenFunc::is_empty(int mul){
    return max_n <= min_n;
}

void GenFunc::pack(int preentry_bits, int index_bits){
    if(!USE_PACKING){
        return;
    }
//    this->pgf = new PackedGenFunc(this, preentry_bits, index_bits);
//    delete[](this->g_func);
//    this->g_func = nullptr;
}


void GenFunc::unpack(int preentry_bits, int index_bits){
    if(!USE_PACKING){
        return;
    }
//    print_bit_array(pgf->bit_array);
    for (int i = 0; i < n+1; ++i) {
        g_func[i] = 0;
    }

    int pos = 0;
    int entries = pgf->get(0, index_bits);
    pos += index_bits;
    for (int i = 0; i < entries; ++i) {
        int index = pgf->get(pos, index_bits);
        pos += index_bits;
        int x_bits = pgf->get(pos, preentry_bits);
        pos += preentry_bits;
        if(x_bits > 64){
            g_func[index].hi = pgf->get(pos, x_bits-64);
            pos += x_bits-64;
            x_bits=64;
        }
        g_func[index].lo = pgf->get(pos, x_bits);
        pos += x_bits;
    }
    delete(pgf);
    pgf = nullptr;
}

GenFunc& GenFunc::operator=(const GenFunc& gf2)
{
    n = gf2.n;
    int target_n = min(n+1, gf2.max_n);
    for (int i = min_n; i < target_n; ++i) {
        g_func[i] = gf2.g_func[i];
    }
    pgf = nullptr;
    max_n = target_n;
    min_n = gf2.min_n;
    return *this;
}

u128_addable GenFunc::at(int i) {
    return g_func[i];
}

void GenFunc::set_at(int i, u128_addable x) {
    g_func[i] = x;
    min_n = min(min_n, i);
    max_n = max(max_n, i+1);
}

void GenFunc::clear_from(int i) {
    max_n = i;
}

int GenFunc::size() {
    return n;
}

bool GenFunc::is_valid() {
    return g_func[min_n].lo != 0 && g_func[max_n-1].lo != 0;
}


bool is_equal(GenFunc& gf1, GenFunc& gf2){
    if(gf1.size() != gf2.size()){
        return false;
    }
    for (int i = 0; i < gf1.size()+1; ++i) {
        if(gf1.at(i).lo != gf2.at(i).lo || gf1.at(i).hi != gf2.at(i).hi){
            return false;
        }
    }
    return true;
}

PackedGenFunc::PackedGenFunc(GenFunc* gf, int preentry_bits, int index_bits){
    int bit_sum = index_bits;
    int element_bits[gf->size()+1];
    int c = 0;
    for (int i = 0; i < gf->size()+1; ++i) {
        if(gf->at(i)){
            element_bits[i] = gf->at(i).bit_size();
            bit_sum += element_bits[i] + preentry_bits + index_bits;
            c++;
        } else {
            element_bits[i] = 0;
        }
    }
    int num_of_entries = (bit_sum-1) / 64 + 2; // +1 to round up, +1 to avoid overflow
    bit_array = new unsigned long long [num_of_entries];
    for (int i = 0; i < num_of_entries; ++i) {
        bit_array[i] = 0;
    }
    int pos = 0;
    insert(pos, c, index_bits);
    pos += index_bits;
    for (int i = 0; i < gf->size()+1; ++i) {
        if(gf->at(i)){
            insert(pos, i, index_bits);
            pos += index_bits;
            insert(pos, element_bits[i], preentry_bits);
            pos += preentry_bits;
            if(gf->at(i).hi){
                insert(pos, gf->at(i).hi, element_bits[i]-64);
                pos += element_bits[i]-64;
                element_bits[i] = 64;
            }
            insert(pos, gf->at(i).lo, element_bits[i]);
            pos += element_bits[i];
        }
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
//    print_bit_array(bit_array);
}

unsigned long long PackedGenFunc::get(int pos, int x_bits) {
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
