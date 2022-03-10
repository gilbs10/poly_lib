//
// Created by gilbe on 13.2.2022.
//

#include "SigDict.h"

SigDict::SigDict(){
    sigs = new sig_map();
    psd = nullptr;
}

SigDict::~SigDict(){
    if(sigs){
        for(auto &sig_it: *sigs){
            delete(sig_it.second);
        }
    }
    delete sigs;
    delete psd;
}

void SigDict::add(sig sig_num, GenFunc &gf, bool new_cell){
    if(gf.is_empty(new_cell)){
        return;
    }
    auto it = sigs->find(sig_num);
    if(it == sigs->end()){
        (*sigs)[sig_num] = new GenFunc();
    }
    (*sigs)[sig_num]->add(gf, new_cell);
}

bool SigDict::is_empty(){
    return sigs->size() == 0;
}

unsigned long long SigDict::size() const{
    if(sigs){
        return sigs->size();
    }
    return psd->get(0, NUM_OF_SIGS_BITS);
}

bool SigDict::operator<(const SigDict &other) const {
    return size() < other.size();
}

/* Structure: (num_of_sigs[NUM_OF_SIGS_BITS], signatures).
 * For each signature (sig_len[PREENTRY_BITS], sig[sig_len], PackedGF)
 * For PackedGF see genfunc.cpp */
void SigDict::pack() {
    unsigned long long bit_sum = NUM_OF_SIGS_BITS; //Num of signatures
    for(auto &it: *sigs){
        bit_sum += PREENTRY_BITS; // sinature length
        bit_sum += bit_size_64(it.first); // sinature length
        bit_sum += it.second->bit_size();
    }
    psd = new PackedArray(bit_sum);
    int pos = 0;
    psd->insert(pos, size(), NUM_OF_SIGS_BITS);
    pos += NUM_OF_SIGS_BITS;
    for(auto &it: *sigs){
        pos += psd->insert(pos, bit_size_64(it.first), PREENTRY_BITS);
        pos += psd->insert(pos, it.first, bit_size_64(it.first));
        pos += psd->insert(pos, *it.second->pgf, it.second->pgf->bit_sum);
        delete it.second;
    }
    delete sigs;
    sigs = nullptr;
}

void SigDict::unpack() {
    if(!psd){
        return;
    }
    int num_of_sigs;
    int pos = 0;
    pos += psd->fetch(pos, &num_of_sigs, NUM_OF_SIGS_BITS);
    int sig_len;
    sig sig_num;
    GenFunc gf;
    sigs = new sig_map();
    for (int i = 0; i < num_of_sigs; ++i) {
        pos += psd->fetch(pos, &sig_len, PREENTRY_BITS);
        pos += psd->fetch(pos, &sig_num, sig_len);
        (*sigs)[sig_num] = new GenFunc(*psd, pos);;
        pos += (*sigs)[sig_num]->pgf->bit_sum;
    }
    delete psd;
    psd = nullptr;
}

void SigDict::allocate(int bit_size, int num_of_elements) {
    delete sigs;
    sigs = nullptr;
    psd = new PackedArray(bit_size);
    packed_pos += psd->insert(0, num_of_elements, NUM_OF_SIGS_BITS);
}

void SigDict::append(sig sig_num, GenFunc &gf) {
    cout << sig_num << endl;
    cout <<gf.pgf->bit_sum << " " << gf.pgf->bit_array[0] << endl;
    packed_pos += psd->insert(packed_pos, bit_size_64(sig_num), PREENTRY_BITS);
    packed_pos += psd->insert(packed_pos, sig_num, bit_size_64(sig_num));
    packed_pos += psd->insert(packed_pos, *gf.pgf, gf.pgf->bit_sum);
}