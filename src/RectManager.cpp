//
// Created by gilbe on 13.2.2022.
//

#include "RectManager.h"
#include <omp.h>
#include <algorithm>
#include <iostream>
#include <numeric>
#include "settings.h"

//
// Created by gilbe on 13.2.2022.
//

using namespace std;

const int UP = 0;
const int DOWN = 1;

enum GlobalChange{NO_FUNC, ADD, ILLEGAL, CLOSE_UPPER_EDGE, CLOSE_LOWER_EDGE, CONNECT_LOWER, CONNECT_UPPER};

// Constant arrays for the changes to be done. Implemented using separate arrays for optimization.
const int LOCAL_CHANGES_TOP[5][5][2] = {
        {{0,1},{0,2},{0,2},{0,3},{0,3}},
        {{0,1},{0,2},{0,2},{0,3},{0,3}},
        {{0,2},{0,2},{0,2},{0,2},{0,2}},
        {{0,3},{0,3},{0,3},{0,3},{0,3}},
        {{0,4},{0,3},{0,3},{0,3},{0,3}}
};
const int LOCAL_CHANGES_BOTTOM[5][5][2] = {
        {{0,0},{1,4},{2,3},{3,3},{4,4}},
        {{0,0},{0,4},{0,3},{0,3},{0,4}},
        {{0,0},{1,3},{2,3},{2,3},{1,4}},
        {{0,0},{1,3},{2,3},{3,3},{4,4}},
        {{0,0},{1,4},{2,3},{3,3},{4,4}}
};
const GlobalChange GLOBAL_CHANGES[5][5][2] = {
        {{NO_FUNC,NO_FUNC},{NO_FUNC,NO_FUNC},{NO_FUNC,NO_FUNC},{NO_FUNC,NO_FUNC},{NO_FUNC,NO_FUNC}},
        {{ADD,NO_FUNC},{ILLEGAL,NO_FUNC},{ILLEGAL,NO_FUNC},{ILLEGAL,NO_FUNC},{ILLEGAL,NO_FUNC}},
        {{CLOSE_LOWER_EDGE,NO_FUNC},{CLOSE_LOWER_EDGE,NO_FUNC},{CLOSE_LOWER_EDGE,CONNECT_LOWER},{NO_FUNC,NO_FUNC},{NO_FUNC,NO_FUNC}},
        {{NO_FUNC,NO_FUNC},{NO_FUNC,NO_FUNC},{NO_FUNC,CONNECT_LOWER},{NO_FUNC,NO_FUNC},{NO_FUNC,NO_FUNC}},
        {{CLOSE_UPPER_EDGE,NO_FUNC},{CLOSE_UPPER_EDGE,NO_FUNC},{CLOSE_UPPER_EDGE,NO_FUNC},{CLOSE_UPPER_EDGE,CONNECT_UPPER},{CLOSE_UPPER_EDGE,CONNECT_UPPER}}
};



RectManager::RectManager(int w, int n, bool white_mode) : status(w, n, white_mode), null_gf(){
    null_gf.set_at(0, 1);
    res = new unordered_map<int, GenFunc*>();
    counter = new SigDict();
    prev_counter = new SigDict();
    sig_counter = 0;
}

RectManager::RectManager(RectStatus status) : status(status), null_gf(){
    null_gf.set_at(0, 1);
    res = new unordered_map<int, GenFunc*>();
    counter = new SigDict();
    prev_counter = new SigDict();
    sig_counter = 0;
}

RectManager::~RectManager(){
    for(auto& it: *res){
        delete it.second;
    }
    delete res;
    delete counter;
//    delete prev_counter; // Used from outside, delete manually.
}

void RectManager::inc_cell(){
    if(status.k < status.w and ((status.col == 0 and not status.white_mode) or (status.col == 1 and status.white_mode))){
        add_seed();
    }
    status.k += 2;
    if(status.k > status.w){
        inc_col();
    }
    status.k_pos = status.k / 2;
    switch_counters();
}


void RectManager::switch_counters(){
    delete prev_counter;
    prev_counter = counter;
    counter = new SigDict();
}

void RectManager::inc_col(){
    status.col += 1;
    status.big_col = status.col % 2 == 0;
    status.k = status.col % 2;
}

void RectManager::add_seed(){
    BoundaryPattern* bp0 = new BoundaryPattern(0, status.pat_length);
    BoundaryPattern* bp = get_new_sig(bp0, 1);
    delete bp0;
    count_bp(counter, bp, null_gf, 1);
    delete bp;
}

void RectManager::count_bp(SigDict* sd, BoundaryPattern* bp, GenFunc &gf, bool new_cell){
    sd->add(bp->get_sig_num(), gf, new_cell);
}

void RectManager::count_res(GenFunc &gf){
    int cur_col = status.col - status.white_mode;
    if(res->find(cur_col) == res->end()){
        (*res)[cur_col] = new GenFunc();
    }
    (*res)[cur_col]->add(gf);
}


void RectManager::filter_gf(GenFunc &gf, BoundaryPattern* bp){

    int h_dist = max(status.w - status.col + status.white_mode - 1, 0);
    int v_dist = bp->border_dist(status);
    pair<int, int> internal_dist = bp->internal_dist(status);
    int total_internal_dist = internal_dist.first;
    int max_internal_dist = internal_dist.second;

    int dist = max(max(h_dist, h_dist -((max_internal_dist-1)/2 + 1)+total_internal_dist), v_dist+total_internal_dist);
//    int dist = max(h_dist, v_dist);
    gf.clear_from(status.n-dist+1);
}

void RectManager::close_edge(BoundaryPattern* bp, int direction, int balance_init){
    int s, t, r0, rn, d, c;
    if(direction == DOWN){
        s = 2;
        t = 4;
        r0 = status.k_pos + 2;
        rn = status.pat_length;
        d = 1;
    } else {
        s = 4;
        t = 2;
        r0 = status.k_pos - 1;
        rn = -1;
        d = -1;
    }
    int balance = 0 + balance_init == s;
    for (int i = r0; d == 1 ? i < rn : i > rn ; i += d) {
        c = bp->pattern[i];
        if(c == s){
            balance += 1;
        }
        else if(c == 3 && balance == 0){
            bp->pattern[i] = s;
            break;
        }
        else if(c == t and balance == 0){
            bp->pattern[i] = 1;
            break;
        }
        else if(c == t) {
            balance -= 1;
        }
    }
}


void RectManager::connect(BoundaryPattern* bp, int direction){
    int s, t, r0, rn, d, c;
    if(direction == DOWN){
        s = 2;
        t = 4;
        r0 = status.k_pos + 2;
        rn = status.pat_length;
        d = 1;
    } else {
        s = 4;
        t = 2;
        r0 = status.k_pos - 1;
        rn = -1;
        d = -1;
    }
    int balance = 0;
    for (int i = r0; d == 1 ? i < rn : i > rn ; i += d) {
        c = bp->pattern[i];
        if(c == s){
            balance += 1;
        }
        else if(c == t and balance == 0){
            bp->pattern[i] = 3;
            break;
        }
        else if(c == t) {
            balance -= 1;
        }
    }
}

void RectManager::fix_bp(BoundaryPattern* bp){
    if(!status.big_col && status.k>status.w-2){
        int prev_entry = 0;
        int temp;
        for (int i = 0; i < status.pat_length; ++i) {
            temp = bp->pattern[i];
            bp->pattern[i] = prev_entry;
            prev_entry = temp;

        }
    }
}

BoundaryPattern* RectManager::get_new_sig(BoundaryPattern* bp, bool new_cell){
    int upper_kink = bp->pattern[status.k_pos];
    int lower_kink = bp->pattern[status.k_pos+1];
    int new_upper_kink = LOCAL_CHANGES_TOP[upper_kink][lower_kink][new_cell];
    int new_lower_kink = LOCAL_CHANGES_BOTTOM[upper_kink][lower_kink][new_cell];
    BoundaryPattern* new_bp = new BoundaryPattern(bp);
    new_bp->pattern[status.k_pos] = new_upper_kink;
    new_bp->pattern[status.k_pos+1] = new_lower_kink;
    new_bp->top_border |= (status.k == 0 && new_cell);
    new_bp->bottom_border |= (status.k == status.w-1 && new_cell);
    switch(GLOBAL_CHANGES[upper_kink][lower_kink][new_cell]){
        case ADD:
            new_bp->disconnected = true;
            break;
        case CLOSE_UPPER_EDGE:
            close_edge(new_bp, UP, 0);
            break;
        case CLOSE_LOWER_EDGE:
            close_edge(new_bp, DOWN, lower_kink);
            break;
        case CONNECT_UPPER:
            connect(new_bp, UP);
            break;
        case CONNECT_LOWER:
            connect(new_bp, DOWN);
            break;
        case ILLEGAL:
            delete new_bp;
            return new BoundaryPattern(0, 1, true); // Illegal pattern
    }
    fix_bp(new_bp);
    return new_bp;
}

void RectManager::run_rectangle(){
    inc_cell();
    while(!prev_counter->is_empty() or status.col < 2){
        process_sigdict(prev_counter, counter);
        inc_cell();
    }
}

void RectManager::process_sigdict(SigDict* prev, SigDict* next) {
    for (auto &it: *(prev->sigs)) {
        sig_counter += 1;
        sig sig_num = it.first;
        BoundaryPattern *bp = new BoundaryPattern(sig_num, status.pat_length);
        GenFunc* gf = it.second;
        gf->unpack();
        filter_gf(*gf, bp);
        if (gf->is_empty()) {
            delete (bp);
            gf->pack();
            continue;
        }
        for (int i = 0; i < 2; ++i) {
            if (status.k == status.w && i == 1) {
                break;
            }
            BoundaryPattern *new_bp = get_new_sig(bp, i);
            if (new_bp->disconnected) {
                if (new_bp->sealed()) {
                    count_res(*gf);
                }
            } else {
                count_bp(next, new_bp, *gf, i);
            }
            delete (new_bp);
        }
        delete(it.second);
        (*prev->sigs)[it.first] = nullptr;
        delete (bp);
    }
}

/* Partial runs */

PartialRectManager::PartialRectManager(RectStatus status1, SigDict *starting_counter, int target_col, int target_k_pos)
                  : RectManager(status1) {
    this->target_col = target_col;
    this->target_k_pos = target_k_pos;
    delete prev_counter;
    prev_counter = starting_counter;
    seeder = false;
}

void PartialRectManager::run_rectangle(){
    prev_counter->unpack();
    while(status.col < target_col || (status.col == target_col && status.k_pos < target_k_pos)){
        process_sigdict(prev_counter, counter);
        inc_cell();
    }
    prev_counter->pack();
}

void PartialRectManager::set_seeder(){
    seeder = true;
}

void PartialRectManager::add_seed(){
    if(!seeder){
        return;
    }
    BoundaryPattern* bp0 = new BoundaryPattern(0, status.pat_length);
    BoundaryPattern* bp = get_new_sig(bp0, 1);
    delete bp0;
    count_bp(counter, bp, null_gf, 1);
    delete bp;
}


/* Parallel implementation*/

RectManagerParallel::RectManagerParallel(int w, int n, bool white_mode, int threads) : status(w, n, white_mode){
    num_of_threads = threads;
    counters = new vector<SigDict*>(1, nullptr);
    col_part = 0;
    res = new unordered_map<int, GenFunc*>;
    sig_counter = 0;
}


RectManagerParallel::RectManagerParallel(int w, int n, int col, bool white_mode, int threads) : status(w, n, white_mode){
    num_of_threads = threads;
    status.col = col;
    counters = new vector<SigDict*>(0);
    for (const auto & entry : filesystem::directory_iterator(PAS_SWAP_PATH)){
        cout << entry << endl;
        cout << entry.path() << endl;
        cout << entry.path().extension() << endl;
        if(entry.path().extension() == "."+PAS_SWAP_EXT) {
            counters->push_back(new SigDict(entry.path()));
        }
    }
    col_part = 0;
    res = new unordered_map<int, GenFunc*>;
    sig_counter = 0;
}

RectManagerParallel::~RectManagerParallel() {
    delete counters;
    for(auto &gf_it: *res){
        delete gf_it.second;
    }
    delete res;
}

int RectManagerParallel::next_target_k_pos() {
    if(col_part == get_flexible_redist_freq()-1){
        return 0;
    } else {
        return ((col_part+1)*status.pat_length)/get_flexible_redist_freq();
    }
}

int RectManagerParallel::next_traget_col() {
    if(col_part == get_flexible_redist_freq()-1){
        return status.col +1;
    } else {
        return status.col;
    }
}


bool PRMpointerGreater(PartialRectManager* prm1, PartialRectManager* prm2){
    return prm1->prev_counter->size() > prm2->prev_counter->size();
}

void RectManagerParallel::run_rectangle(){
    // First column is processed separately
    int target_col;
    if(status.white_mode){
        target_col = 2;
    } else {
        target_col = 1;
    }
    PartialRectManager* temp_rm = new PartialRectManager(status, new SigDict(), target_col, 0);
    temp_rm->set_seeder();
    temp_rm->run_rectangle();
    (*counters)[0] = temp_rm->prev_counter;
    status = temp_rm->status;
    delete temp_rm;
    vector<PartialRectManager*>* managers = new vector<PartialRectManager*>;
    while((!is_empty_counters() or status.col < 2) and status.col < RM_TARGET_COL){
        if(PRINT_RES_BY_COL){
            cout << FORMAT_TITLE("RUNNING COL");
            cout << FORMAT_ATTR("Size", status.n);
            cout << FORMAT_ATTR("Width", status.w);
            cout << FORMAT_ATTR("Col", status.col);
            cout << FORMAT_ATTR("Row", status.k);
            cout << endl;
        }
        redistribute_sigs();
        cout << FORMAT_TITLE_VERBOSE("END REDIST SIGS");
        cout << endl;

        managers->clear();
        for(auto &counters_it: *counters){
            if(counters_it){
                managers->push_back(new PartialRectManager(status, counters_it, next_traget_col(), next_target_k_pos()));
            }
        }
        sort(managers->begin(), managers->end(), PRMpointerGreater);
        if(FIXED_NUM_OF_THREADS){
            omp_set_num_threads(get_flexible_num_of_threads());
        }
        #pragma omp parallel for schedule(dynamic, 1) default(none) shared(managers)
        for (int i = 0; i < managers->size(); ++i) {
            (*managers)[i]->run_rectangle();
        }
        counters->clear();
        if(managers->size() == 0){
            break;
        }
        status = (*managers)[0]->status;
        counters->resize(managers->size());
        for (unsigned long long i = 0; i < managers->size(); ++i) {
            sig_counter += (*managers)[i]->sig_counter;
            count_res((*managers)[i]->res);
            (*counters)[i] = (*managers)[i]->prev_counter;
            delete (*managers)[i];
        }
        col_part += 1;
        col_part %= get_flexible_redist_freq();
    }
    delete managers;
}

bool RectManagerParallel::is_empty_counters(){
    return counters->size() == 0;
}


void RectManagerParallel::redistribute_sigs(){
    cout << FORMAT_TITLE_VERBOSE("REDIST SIGS");
    cout << endl;
    int c= 0;
    sig occupancy_num;
    int s,t;
    s = status.k_pos;
    t = next_target_k_pos();
    if(t == 0){
        t = status.pat_length;
    }
    int occ_num = 1<<(status.pat_length - (t-s));
    unsigned long long persistant_size_threshold = 0;
    vector<SigDict*>* temp_counters = new vector<SigDict*>(occ_num, nullptr);
    vector<unsigned long long>* counters_packed_sizes = new vector<unsigned long long>(occ_num, NUM_OF_SIGS_BITS);
    vector<unsigned long long>* counters_size = new vector<unsigned long long>(occ_num, 0);
    vector<omp_lock_t>* counters_locks = new vector<omp_lock_t>(occ_num);
    for (int i = 0; i < counters_locks->size(); ++i) {
        omp_init_lock(&((*counters_locks)[i]));
    }
#ifndef SD_PACK_TO_FILE
    cout << FORMAT_TITLE_VERBOSE("SUM SIZE");
    cout << FORMAT_ATTR_VERBOSE("start_occ", s);
    cout << FORMAT_ATTR_VERBOSE("end_occ", t);
    cout << endl;
#pragma omp parallel for schedule(dynamic, 1) default(none) shared(counters, counters_packed_sizes, counters_size, counters_locks, s, t, cout)
    for(auto counters_it = counters->begin(); counters_it != counters->end(); counters_it++){
        (*counters_it)->unpack();
        for(auto sig_it = (*counters_it)->sigs->begin(); sig_it != (*counters_it)->sigs->end(); sig_it++){
            BoundaryPattern bp = BoundaryPattern(sig_it->first, status.pat_length);
            if (status.w % 2 && status.k_pos == 0 && USE_REVERSING){
                if (bp.get_sig_num() > bp.get_reverse_sig_num()){
                    bp.reverse(status.col %2);
                }
            }
            sig occupancy_num = bp.get_occupancy_num(s, t);
            omp_set_lock(&((*counters_locks)[occupancy_num]));
            (*counters_packed_sizes)[occupancy_num] += (PREENTRY_BITS
                                                        + bit_size_n(bp.get_sig_num())
                                                        + sig_it->second->bit_size());
            (*counters_size)[occupancy_num] += 1;
            omp_unset_lock(&((*counters_locks)[occupancy_num]));
        }
        (*counters_it)->pack();
    }
    persistant_size_threshold = accumulate(counters_packed_sizes->begin(), counters_packed_sizes->end(), 0);
    persistant_size_threshold *= SD_PERSISTANT_THRESHOLD;
    cout << FORMAT_TITLE_VERBOSE("SIZE_SUMMED");
    cout << FORMAT_ATTR_VERBOSE("max_bit_size", *max_element(counters_packed_sizes->begin(), counters_packed_sizes->end()));
    cout << FORMAT_ATTR_VERBOSE("max_size", *max_element(counters_size->begin(), counters_size->end()));
    cout << FORMAT_ATTR_VERBOSE("sum_bit_size", accumulate(counters_packed_sizes->begin(), counters_packed_sizes->end(), 0));
    cout << FORMAT_ATTR_VERBOSE("sum_size", accumulate(counters_size->begin(), counters_size->end(), 0));
    cout << endl;
#endif
#pragma omp parallel for schedule(dynamic, 1) default(none) shared(counters, temp_counters, counters_packed_sizes, counters_size, counters_locks, s, t, cout, persistant_size_threshold)
    for(auto counters_it = counters->begin(); counters_it != counters->end(); counters_it++){
        (*counters_it)->unpack();
        for(auto sig_it = (*counters_it)->sigs->begin(); sig_it != (*counters_it)->sigs->end(); sig_it++){
            bool use_rev = false;
            BoundaryPattern bp = BoundaryPattern(sig_it->first, status.pat_length);
            if (status.w % 2 && status.k_pos == 0 && USE_REVERSING){
                if (bp.get_sig_num() > bp.get_reverse_sig_num()){
                    bp.reverse(status.col %2);
                }
            }
            sig occupancy_num = bp.get_occupancy_num(s, t);
            omp_set_lock(&((*counters_locks)[occupancy_num]));
            if((*temp_counters)[occupancy_num] == nullptr){
                (*temp_counters)[occupancy_num] = new SigDict();
                if(SD_USE_PACKING){
#ifdef SD_PACK_TO_FILE
                    if(SD_USE_PERSISTANT && (*counters_packed_sizes)[occupancy_num] > persistant_size_threshold){
                        (*temp_counters)[occupancy_num]->persistant = true;
                    }
#endif
                    (*temp_counters)[occupancy_num]->allocate((*counters_packed_sizes)[occupancy_num], (*counters_size)[occupancy_num]);
                }
            }
            if(SD_USE_PACKING){
                (*temp_counters)[occupancy_num]->append(bp.get_sig_num(), *sig_it->second);
            }
            else{
                (*temp_counters)[occupancy_num]->add(bp.get_sig_num(), *sig_it->second, 0);
            }
            delete(sig_it->second);
            (*(*counters_it)->sigs)[sig_it->first] = nullptr;
            omp_unset_lock(&((*counters_locks)[occupancy_num]));
        }
        delete *counters_it;
    }
    delete counters;
    delete counters_locks;
    delete counters_size;
    delete counters_packed_sizes;
    counters = temp_counters;
}

void RectManagerParallel::count_res(unordered_map<int, GenFunc*>* res_to_add){
    for(auto& it: *res_to_add){
        if(res->find(it.first) == res->end()){
            (*res)[it.first] = new GenFunc();
        }
        (*res)[it.first]->add(*it.second);
    }
}

int RectManagerParallel::get_flexible_num_of_threads() {
//    if(status.col <26){
//        return 31;
//    }
//    return 12;
    return num_of_threads;
}

int RectManagerParallel::get_flexible_redist_freq() {
//    if(status.col <26){
//        return 7;
//    }
//    return 2;
    return RM_REDIST_FREQ;
}