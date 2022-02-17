//
// Created by gilbe on 13.2.2022.
//

#include "RectManager.h"


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

RectStatus::RectStatus(int w, int n, bool white_mode) {
    this->w = w;
    this->n = n;
    this->white_mode = white_mode;
    this->odd_width = bool(w % 2);
    this->pat_length = w / 2 + 1;
    this->col = 0;
    this->k = 0;
    this->k_pos = 0;
    this->big_col = true;
}

RectManager::RectManager(int w, int n, bool white_mode) : status(w, n, white_mode){
    null_gf = new GenFunc(n);
    null_gf->g_func[0] = 1;
    res = new unordered_map<int, GenFunc*>();
    counter = new SigDict();
    prev_counter = new SigDict();
}

RectManager::RectManager(RectStatus status) : status(status){
    null_gf = new GenFunc(status.n);
    null_gf->g_func[0] = 1;
    res = new unordered_map<int, GenFunc*>();
    counter = new SigDict();
    prev_counter = new SigDict();
}

RectManager::~RectManager(){
    delete null_gf;
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

void RectManager::count_bp(SigDict* sd, BoundaryPattern* bp, GenFunc* gf, bool new_cell){
    sd->add(bp->get_sig_num(), gf, new_cell);
}

void RectManager::count_res(GenFunc* gf){
    int cur_col = status.col - status.white_mode;
    if(res->find(cur_col) == res->end()){
        (*res)[cur_col] = new GenFunc(status.n);
    }
    (*res)[cur_col]->add(gf);
}

int RectManager::pat_pos_to_k(int i){
    return (i  * 2) - status.big_col + (i < status.k_pos);
}

void RectManager::filter_gf(GenFunc* gf, BoundaryPattern* pat){
    int h_dist = status.w - status.col + status.white_mode - 1; // TODO: verify
    int v_dist = 0;
    int i;
    if(!pat->top_border){
        i = 0;
        while(pat->pattern[i] == 0){
            i++;
        }
        v_dist += (pat_pos_to_k(i));
    }
    if(!pat->bottom_border){
        i = status.pat_length-1;
        while(pat->pattern[i] == 0){
            i--;
        }
        v_dist += (status.w - pat_pos_to_k(i) - 1);
    }
    int dist = max(h_dist, v_dist);
    for (int j = status.n-dist+1; j < status.n+1; ++j) {
        gf->g_func[j] = 0;
    }
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
        sig sig_num = it.first;
        BoundaryPattern *bp = new BoundaryPattern(sig_num, status.pat_length);
        GenFunc *gf = it.second;
        gf->unpack();
        filter_gf(gf, bp);
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
                    count_res(gf);
                }
            } else {
                count_bp(next, new_bp, gf, i);
            }
            delete (new_bp);
        }
        gf->pack();
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
    while(status.col < target_col || (status.col == target_col && status.k_pos < target_k_pos)){
        process_sigdict(prev_counter, counter);
        inc_cell();
    }
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
    counters = new unordered_map<sig, SigDict*>();
    top_half = true;
    res = new unordered_map<int, GenFunc*>;
}

RectManagerParallel::~RectManagerParallel() {
    delete counters;
    for(auto &gf_it: *res){
        delete gf_it.second;
    }
    delete res;
}

int RectManagerParallel::next_target_k_pos() {
    if(!top_half){
        return 0;
    } else {
        return status.pat_length/2;
    }
}

int RectManagerParallel::next_traget_col() {
    if(!top_half){
        return status.col +1;
    } else {
        return status.col;
    }
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
    vector<PartialRectManager*> managers;
    thread_pool pool(num_of_threads);
    while(!is_empty_counters() or status.col < 2){
        redistribute_sigs();
        managers.clear();
        for(auto &counters_it: *counters){
            managers.push_back(new PartialRectManager(status, counters_it.second, next_traget_col(), next_target_k_pos()));
        }
        for (auto &managers_it: managers) {
            pool.push_task([managers_it]{managers_it->run_rectangle();});
        }
        pool.wait_for_tasks();
        counters->clear();
        if(managers.size() == 0){
            break;
        }
        status = managers[0]->status;
        for (unsigned long long i = 0; i < managers.size(); ++i) {
            count_res(managers[i]->res);
            (*counters)[i] = managers[i]->prev_counter;
            delete managers[i];
        }
        top_half = !top_half;
    }
}

bool RectManagerParallel::is_empty_counters(){
    return counters->size() == 0;
}

void RectManagerParallel::redistribute_sigs(){
    int c= 0;
    sig occupancy_num;
    int s,t;
    unordered_map<sig, SigDict*>* temp_counters = new unordered_map<sig, SigDict*>();
    if(!top_half){
        s = 0;
        t = status.pat_length/2;
    } else {
        s = status.pat_length/2;
        t = status.pat_length;
    }

    for(auto &counters_it: *counters){
        for(auto &sig_it: *(counters_it.second->sigs)){
            sig occupancy_num = BoundaryPattern(sig_it.first, status.pat_length).get_occupancy_num(s, t);
            if(temp_counters->find(occupancy_num) == temp_counters->end()){
                (*temp_counters)[occupancy_num] = new SigDict();
            }
            (*temp_counters)[occupancy_num]->add(sig_it.first, sig_it.second, 0);
        }
    }
    for(auto &counters_it: *counters){
        delete counters_it.second;
    }
    delete counters;
    counters = temp_counters;
}

void RectManagerParallel::count_res(unordered_map<int, GenFunc*>* res_to_add){
    for(auto& it: *res_to_add){
        if(res->find(it.first) == res->end()){
            (*res)[it.first] = new GenFunc(status.n);
        }
        (*res)[it.first]->add(it.second);
    }
}