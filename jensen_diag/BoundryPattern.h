//
// Created by gilbe on 13.2.2022.
//

#ifndef JENSEN_DIAG_BOUNDRYPATTERN_H
#define JENSEN_DIAG_BOUNDRYPATTERN_H

#include <functional>
#include "RectStatus.h"
#include <stack>
#include <queue>
#include "set"
#include "settings.h"

using namespace std;

#ifdef LIMIT_SIG_64
typedef unsigned long long int sig;
#else
typedef __int128 sig;
#endif

class BoundaryPattern{
    /* A modifyable pattern, right now it is optimize to minimize memory consumption, i.e. using log_2(5) bits per
     * boundary cell.
     * TOOO: Things to try:
     * 1) Optimize to time using 3 bits per entry.
     * 2) Work directly on the number instead of an array.*/

public:
    int* pattern;
    bool top_border;
    bool bottom_border;
    bool disconnected;
    int pat_length;
    BoundaryPattern(sig sig_num, int pat_length, bool disconnected = false);
    BoundaryPattern(BoundaryPattern* bp1);
    ~BoundaryPattern();
    sig get_sig_num(int ignore_last = 0);
    sig get_reverse_sig_num(int ignore_first = 0);
    sig get_occupancy_num(int s, int t);
    void reverse(bool gray_col);
    pair<int, int> internal_dist(RectStatus& status);
    int border_dist(RectStatus& status);
    int pos2k(int pos, RectStatus& status);
    bool sealed();
};

struct Edge{
    int target;
    int weight;
    bool operator<(const Edge& e);
    bool operator>(const Edge& e);
};

class BoundaryGraph{
public:
    // Ugly, but much faster than stl containers since it allows on-demand initialization.
    // Finding the size of the graph beforehand is expensive in our case.
    int num_of_nodes;
    int* node_degree;
    Edge* adj_mat;
    int max_n;
    BoundaryGraph(int pat_length);
    ~BoundaryGraph();
    void add_node();
    void insert(int s, int t, int d);
    void insert_directed(int s, int t, int d);
    pair<int, int> get_mst_weight();
};


#endif //JENSEN_DIAG_BOUNDRYPATTERN_H
