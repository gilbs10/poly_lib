//
// Created by gilbe on 13.2.2022.
//

#ifndef JENSEN_DIAG_SIGDICT_H
#define JENSEN_DIAG_SIGDICT_H

#include "BoundryPattern.h"
#include "genfunc.h"
#include <unordered_map>

class SigDict{
public:
    unordered_map<sig, GenFunc*>* sigs;
    int target_n;
    SigDict(int n);
    SigDict();
    ~SigDict();
    void add(sig sig_num, GenFunc* gf, bool new_cell);
    bool is_empty();
    // TODO iterator
};

#endif //JENSEN_DIAG_SIGDICT_H
