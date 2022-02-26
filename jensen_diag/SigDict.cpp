//
// Created by gilbe on 13.2.2022.
//

#include "SigDict.h"

SigDict::SigDict(){
    sigs = new sig_map();
}

SigDict::~SigDict(){
    for(auto &sig_it: *sigs){
        delete(sig_it.second);
    }
    delete sigs;
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
