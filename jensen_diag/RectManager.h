//
// Created by gilbe on 13.2.2022.
//

#ifndef JENSEN_DIAG_RECTMANAGER_H
#define JENSEN_DIAG_RECTMANAGER_H

#include "SigDict.h"
#include "unordered_map"
using namespace std;

struct RectStatus{
public:
    int w;
    int n;
    bool white_mode;
    int k;
    int k_pos;
    int col;
    bool odd_width;
    bool big_col;
    int pat_length;
    RectStatus(int w, int n, bool white_mode);
};

class RectManager{
public:
    RectStatus status;
    SigDict* counter;
    SigDict* prev_counter;
    unordered_map<int, GenFunc*>* res;
    GenFunc* null_gf;
    RectManager(int w, int n, bool white_mode);
    RectManager(RectStatus status1);
    ~RectManager();
    void inc_cell();
    void switch_counters();
    void count_bp(SigDict* sd, BoundaryPattern* bp, GenFunc* gf, bool new_cell);
    void inc_col();
    virtual void add_seed();
    void count_res(GenFunc* gf);
    int pat_pos_to_k(int i);
    void filter_gf(GenFunc* gf, BoundaryPattern* pat);
    void close_edge(BoundaryPattern* bp, int direction, int balance_init);
    void connect(BoundaryPattern* bp, int direction);
    void fix_bp(BoundaryPattern* bp);
    BoundaryPattern* get_new_sig(BoundaryPattern* bp, bool new_cell);
    virtual void run_rectangle();
    void process_sigdict(SigDict* prev, SigDict* next);
};

class PartialRectManager : public RectManager {
public:
    int target_col;
    int target_k_pos;
    bool seeder;
    PartialRectManager(RectStatus status1,  SigDict* counter, int target_col, int target_k_pos);
    void run_rectangle() override;
    void set_seeder();
    void add_seed() override;
};

class RectManagerParallel{
public:
    RectStatus status;
    SigDict** counters;
    unordered_map<int, GenFunc*>* res;
    int num_of_threads;
    bool top_half;
    RectManagerParallel(int w, int n, bool white_mode, int threads);
    ~RectManagerParallel();
    void run_rectangle();
    void count_res(unordered_map<int, GenFunc*>* res);
    bool is_empty_counters();
    void redistribute_sigs();
    int next_traget_col();
    int next_target_k_pos();
};

#endif //JENSEN_DIAG_RECTMANAGER_H
