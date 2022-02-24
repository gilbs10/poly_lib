//
// Created by gilbe on 13.2.2022.
//

#ifndef JENSEN_DIAG_SIGDICT_H
#define JENSEN_DIAG_SIGDICT_H

#include "BoundryPattern.h"
#include "genfunc.h"
#include "robin_hood.h"
#include "abseil-cpp/absl/container/flat_hash_map.h"

//typedef robin_hood::unordered_map<sig, GenFunc*> sig_map;
typedef absl::flat_hash_map<sig, GenFunc*> sig_map;
//typedef unordered_map<sig, GenFunc*> sig_map;

class SigDict{
public:
    sig_map * sigs;
    SigDict();
    ~SigDict();
    void add(sig sig_num, GenFunc &gf, bool new_cell);
    bool is_empty();
    // TODO iterator
};

#endif //JENSEN_DIAG_SIGDICT_H
