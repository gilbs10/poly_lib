//
// Created by gilbe on 13.2.2022.
//

#include "SigDict.h"
#include "utils.h"

SigDict::SigDict(){
    sigs = new sig_map();
    psd = nullptr;
    num_of_elements = 0;
#ifdef SD_PACK_TO_FILE
    persistant = false;
#endif
}

SigDict::SigDict(filesystem::path pas_file){
    sigs = nullptr;
    psd = new PackedArraySwappable(pas_file, false);
    num_of_elements = 0;
#ifdef SD_PACK_TO_FILE
    persistant = false;
#endif
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
#ifdef SD_PACK_TO_FILE
    return num_of_elements;
#else
    return psd->get(0, NUM_OF_SIGS_BITS);
#endif
}

bool SigDict::operator<(const SigDict &other) const {
    return size() < other.size();
}

/* Structure: (num_of_sigs[NUM_OF_SIGS_BITS], signatures).
 * For each signature (sig_len[PREENTRY_BITS], sig[sig_len], PackedGF)
 * For PackedGF see genfunc.cpp */
void SigDict::pack() {
    if(!SD_USE_PACKING){
        return;
    }
    unsigned long long bit_sum = NUM_OF_SIGS_BITS; //Num of signatures
    for(auto &it: *sigs){
        bit_sum += PREENTRY_BITS;
        bit_sum += bit_size_n(it.first);
        bit_sum += it.second->bit_size();
    }
#ifdef SD_PACK_TO_FILE
    psd = new PackedArraySwappable(bit_sum, persistant);
#else
    psd = new PackedArray(bit_sum);
#endif

    int pos = psd->insert(0, size(), NUM_OF_SIGS_BITS);
    for(auto &it: *sigs){
        pos += psd->insert(pos, bit_size_n(it.first), PREENTRY_BITS);
        pos += psd->PackedArray::insert(pos, it.first, bit_size_n(it.first));
        pos += ((PackedArray*)psd)->insert(pos, *it.second->pgf, it.second->pgf->bit_sum);
        delete it.second;
    }
    delete sigs;
    sigs = nullptr;
}

void SigDict::unpack() {
    if(!psd || !SD_USE_PACKING){
        return;
    }
#ifdef SD_PACK_TO_FILE
        psd->unswap();
#endif
    int num_of_sigs;
    int pos = psd->fetch(0, &num_of_sigs, NUM_OF_SIGS_BITS);
    if(num_of_sigs == 0){
        num_of_sigs = num_of_elements;
    }
    int sig_len;
    sig sig_num;
    sigs = new sig_map();
    for (int i = 0; i < num_of_sigs; ++i) {
        pos += psd->fetch(pos, &sig_len, PREENTRY_BITS);
        pos += psd->PackedArray::fetch(pos, &sig_num, sig_len);
        if(sigs->find(sig_num) == sigs->end()){
            (*sigs)[sig_num] = new GenFunc(*psd, pos);;
            pos += (*sigs)[sig_num]->pgf->bit_sum;
        } else{
            GenFunc gf = GenFunc(*psd, pos);

            pos += gf.pgf->bit_sum;
            (*sigs)[sig_num]->add(gf, 0);
        }
    }
    delete psd;
    psd = nullptr;
}

void SigDict::allocate(int bit_size, unsigned long long num_of_elements) {
    delete sigs;
    sigs = nullptr;
#ifdef SD_PACK_TO_FILE
        psd = new PackedArraySwappable(bit_size, persistant);
        this->num_of_elements = num_of_elements;
#else
        psd = new PackedArray(bit_size);
#endif
    packed_pos = psd->insert(0, num_of_elements, NUM_OF_SIGS_BITS);
}

void SigDict::append(sig sig_num, GenFunc &gf) {
    num_of_elements += 1;
    packed_pos += psd->insert(packed_pos, bit_size_n(sig_num), PREENTRY_BITS);
    packed_pos += psd->PackedArray::insert(packed_pos, sig_num, bit_size_n(sig_num));
    packed_pos += ((PackedArray*)psd)->insert(packed_pos, *gf.pgf, gf.pgf->bit_sum);
}
