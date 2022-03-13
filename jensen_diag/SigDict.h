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
    sig_map* sigs;
    PackedArray* psd;
    string packed_file;
    int packed_pos;
    SigDict();
    ~SigDict();
    void add(sig sig_num, GenFunc &gf, bool new_cell);
    void allocate(int bit_size, int num_of_elements);
    void append(sig sig_num, GenFunc &gf);
    bool is_empty();
    unsigned long long size() const;
    void pack();
    void unpack();
    bool operator < (const SigDict& other) const;
};


#endif //JENSEN_DIAG_SIGDICT_H
