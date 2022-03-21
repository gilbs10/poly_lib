//
// Created by gilbe on 9.3.2022.
//

#include "PackedArray.h"

const int PA_CELL_SIZE = 64;

PackedArray::PackedArray() : bit_sum(0), bit_array(nullptr) {
}

PackedArray::PackedArray(unsigned long long bit_sum) : bit_sum(bit_sum) {
    bit_array = new unsigned long long[(bit_sum-1) / PA_CELL_SIZE + 1]();
}

int PackedArray::insert(unsigned long long pos, unsigned long long x, int x_bits){
    int slack = PA_CELL_SIZE - (pos % PA_CELL_SIZE);
    int i = pos / PA_CELL_SIZE;
    if(x_bits > slack){
        bit_array[i] |= x>>(x_bits-slack);
        bit_array[i+1] |= x<<(PA_CELL_SIZE - (x_bits - slack));
    } else {
        bit_array[i] |= x<<(slack-x_bits);
    }
    return x_bits;
}

unsigned long long PackedArray::get(unsigned long long pos, int x_bits) const {
    int slack = PA_CELL_SIZE - (pos % PA_CELL_SIZE);
    int i = pos / PA_CELL_SIZE;
    unsigned long long res = 0;
    if(x_bits > slack){
        res = (bit_array[i]<<(PA_CELL_SIZE - slack)) >> (PA_CELL_SIZE - x_bits);
        res |= bit_array[i+1]>>(PA_CELL_SIZE - (x_bits - slack));

    } else {
        res |= (bit_array[i]<<(PA_CELL_SIZE - slack)) >> (PA_CELL_SIZE - x_bits);
    }
    return res;
}


int PackedArray::insert(unsigned long long pos, u128_addable x, int x_bits){
    int x_bits_old = x_bits;
    if(x.hi){
        insert(pos, x.hi, x_bits - PA_CELL_SIZE);
        pos += x_bits - PA_CELL_SIZE;
        x_bits = PA_CELL_SIZE;
    }
    insert(pos, x.lo, x_bits);
    return x_bits_old;
}

int PackedArray::insert(unsigned long long pos, u192_addable x, int x_bits){
    int x_bits_old = x_bits;
    if(x.hi){
        insert(pos, x.hi, x_bits - PA_CELL_SIZE);
        pos += x_bits - PA_CELL_SIZE;
        x_bits = PA_CELL_SIZE;
    }
    insert(pos, x.lo, x_bits);
    return x_bits_old;
}

int PackedArray::insert(unsigned long long pos, u64_addable_mod x, int x_bits){
    insert(pos, x.x, x_bits);
    return x_bits;
}

int PackedArray::insert(unsigned long long pos, u128_full x, int x_bits){
    insert(pos, x.x, x_bits);
    return x_bits;
}

int PackedArray::insert(unsigned long long pos, PackedArray& pa, int x_bits){
    for (int i = 0; i < pa.array_length() - 1; ++i) {
        pos += insert(pos, pa.bit_array[i], PA_CELL_SIZE);
    }
    int bits = x_bits % PA_CELL_SIZE;
    if(!bits){
        bits = PA_CELL_SIZE;
    }
    unsigned long long last = pa.bit_array[pa.array_length()-1];
    last >>= PA_CELL_SIZE - bits;
    insert(pos, last, bits);

    return x_bits;
}



int PackedArray::fetch(unsigned long long pos, u128_addable* x ,int x_bits) const{
    int x_bits_old = x_bits;
    x->hi = 0;
    if(x_bits > PA_CELL_SIZE){
        x->hi = get(pos, x_bits - PA_CELL_SIZE);
        pos += x_bits - PA_CELL_SIZE;
        x_bits=PA_CELL_SIZE;
    }
    x->lo = get(pos, x_bits);
    return x_bits_old;
}


int PackedArray::fetch(unsigned long long pos, u192_addable* x ,int x_bits) const{
    int x_bits_old = x_bits;
    x->hi = 0;
    if(x_bits > PA_CELL_SIZE){
        x->hi = get(pos, x_bits - PA_CELL_SIZE);
        pos += x_bits - PA_CELL_SIZE;
        x_bits=PA_CELL_SIZE;
    }
    x->lo = get(pos, x_bits);
    return x_bits_old;
}

int PackedArray::fetch(unsigned long long pos, int* x, int x_bits) const{
    *x = get(pos, x_bits);
    return x_bits;
}

int PackedArray::fetch(unsigned long long pos, unsigned long long* x, int x_bits) const{
    *x = get(pos, x_bits);
    return x_bits;
}

int PackedArray::fetch(unsigned long long pos, u64_addable_mod* x, int x_bits) const{
    x->x = get(pos, x_bits);
    return x_bits;
}

int PackedArray::fetch(unsigned long long pos, u128_full* x, int x_bits) const{
    x->x = get(pos, x_bits);
    return x_bits;
}

unsigned long long PackedArray::array_length() {
    return (bit_sum-1) / PA_CELL_SIZE + 1;
}


PackedArray::PackedArray(const PackedArray &other) {
    bit_sum = other.bit_sum;
    bit_array = new unsigned long long[array_length()];
    for (unsigned long long i = 0; i < array_length(); ++i) {
        bit_array[i] = other.bit_array[i];
    }
}

PackedArray::~PackedArray(){
    delete[] bit_array;
}

PackedArray &PackedArray::operator=(const PackedArray &other) {
    bit_sum = other.bit_sum;
    bit_array = new unsigned long long[array_length()];
    for (unsigned long long i = 0; i < array_length(); ++i) {
        bit_array[i] = other.bit_array[i];
    }
    return *this;
}

PackedArraySwappable::PackedArraySwappable(unsigned long long bitsize) {
    bit_sum = bitsize;
    buffer_pos = 0;
    for (int i = 0; i < PAS_BUFFER_SIZE; ++i) {
        bit_buffer[i] = 0;
    }
    open_file();
}

PackedArraySwappable::~PackedArraySwappable() {
    close_file();
}

void PackedArraySwappable::open_file() {
    if(bit_file.is_open()){
       return;
    }
    bit_file_name = get_available_fname(PAS_SWAP_PATH, PAS_SWAP_EXT);
    bit_file.open(bit_file_name ,ios::in | ios::out | ios::app | ios::binary);
}

void PackedArraySwappable::close_file() {
    if(!bit_file.is_open()){
        return;
    }
    bit_file.close();
    filesystem::remove(bit_file_name);
    bit_file_name = "";
}


void PackedArraySwappable::swap_all() {
    open_file();
    bit_file.write(reinterpret_cast<char*>(bit_array), array_length()*sizeof(unsigned long long));
    delete bit_array;
    bit_array = nullptr;
}

void PackedArraySwappable::unswap() {
    swap_buffer();
    delete bit_array;
    bit_array = new unsigned long long[array_length()];
    bit_file.seekg(0);
    bit_file.read(reinterpret_cast<char*>(bit_array), array_length()*sizeof(unsigned long long));
    close_file();
}

void PackedArraySwappable::swap_buffer() {
    bit_file.write(reinterpret_cast<char*>(bit_buffer), buffer_length()*sizeof(unsigned long long));
    bit_file.flush();
    for (int i = 0; i < PAS_BUFFER_SIZE; ++i) {
        bit_buffer[i] = 0;
    }
    buffer_pos = 0;
}

int PackedArraySwappable::insert(unsigned long long pos, unsigned long long x, int x_bits) {
    int slack = PA_CELL_SIZE - (buffer_pos % PA_CELL_SIZE);
    int i = buffer_pos / PA_CELL_SIZE;
    if(x_bits >= slack){
        bit_buffer[i] |= x>>(x_bits-slack);
        buffer_pos += slack;
        if(i+1 >= PAS_BUFFER_SIZE){
            swap_buffer();
            i=-1;
        }
        if(x_bits != slack) {
            bit_buffer[i + 1] |= x << (PA_CELL_SIZE - (x_bits - slack));
            buffer_pos += x_bits - slack;
        }
    } else {
        bit_buffer[i] |= x<<(slack-x_bits);
        buffer_pos += x_bits;
    }
    return x_bits;
}
int PackedArraySwappable::buffer_length() {
    return (buffer_pos-1) / PA_CELL_SIZE + 1;
}
