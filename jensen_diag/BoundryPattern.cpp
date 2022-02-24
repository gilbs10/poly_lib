//
// Created by gilbe on 13.2.2022.
//

#include "BoundryPattern.h"
#include "iostream"

using namespace std;

BoundaryPattern::BoundaryPattern(sig sig_num, int pat_length, bool disconnected){
    this->pat_length = pat_length;
    bottom_border = bool(sig_num % 2);
    sig_num /= 2;
    top_border = bool(sig_num % 2);
    sig_num /= 2;
    pattern = new int[pat_length+1](); // Padded by zero to handle kink in the last position
    for (int i = 0; i < pat_length; ++i) {
        pattern[i] = sig_num%5;
        sig_num /= 5;
    }
    this->disconnected = disconnected;
}
BoundaryPattern::BoundaryPattern(BoundaryPattern* bp1){
    pat_length = bp1->pat_length;
    bottom_border = bp1->bottom_border;
    top_border = bp1->top_border;
    pattern = new int[pat_length+1]();
    for (int i = 0; i < pat_length; ++i) {
        pattern[i] = bp1->pattern[i];
    }
    disconnected = bp1->disconnected;
}
BoundaryPattern::~BoundaryPattern(){
    delete[](pattern);
}
sig BoundaryPattern::get_sig_num(int ignore_last){
    sig sig_num = 0;
    for (int i = pat_length-1-ignore_last; i >= 0; --i) {
        sig_num *= 5;
        sig_num += pattern[i];
    }
    return sig_num*4 + 2*top_border + bottom_border;
}

sig BoundaryPattern::get_reverse_sig_num() {
    sig sig_num = 0;
    for (int i = 0; i < pat_length-1; ++i) {
        sig_num *= 5;
        switch(pattern[i]){
            case 2:
                sig_num+=4;
                break;
            case 4:
                sig_num+=2;
                break;
            default:
                sig_num += pattern[i];
        }
    }
    return sig_num*4 + 2*bottom_border + top_border;
}

bool BoundaryPattern::sealed(){
    return get_sig_num() == 3;
}

sig BoundaryPattern::get_occupancy_num(int s, int t){
    sig occupancy_num = 0;
    for (int i = s; i < t; ++i) {
        occupancy_num *= 2;
        if(pattern[i]){
            occupancy_num += 1;
        }
    }
    return occupancy_num;
}

// Returns min dist to connect all the components and max dist between two components
pair<int, int> BoundaryPattern::internal_dist(RectStatus& status){
    // We use an explicit array instead of stack<int> to prevent the (amortized) cost of resizing, this function is a
    // bottleneck and it improves the running time.
    int component_stack[pat_length];
    int stack_pos = 0;
    BoundaryGraph component_adj_graph(pat_length);
    int last_component = 0;
    int gap_start_comp = -1;
    int cell, gap_start_pos, gap_end_pos, gap_end_comp, dist;
    int start = 0;
    while (pattern[start] == 0){
        start++;
    }
    for (int i = start; i < pat_length; ++i) {
        cell = pattern[i];
        if (!cell){
            continue;
        }
        if(cell == 1 || cell  == 2){
            // Stack push
            component_stack[stack_pos] = last_component;
            stack_pos++;
            last_component++;
        }
        gap_end_pos = i;
        gap_end_comp = component_stack[stack_pos-1];
        if(gap_start_comp != gap_end_comp && gap_start_comp != -1){
            dist = pos2k(gap_end_pos, status) - pos2k(gap_start_pos, status) - 1;
            component_adj_graph.insert(gap_start_comp, gap_end_comp, dist);
        }
        gap_start_pos = gap_end_pos;
        gap_start_comp = gap_end_comp;
        if(cell == 1 || cell  == 4){
            stack_pos--;
        }
    }
    if (last_component > 1){
        return component_adj_graph.get_mst_weight();
    } else {
        return {0, 0};
    }
}

int BoundaryPattern::border_dist(RectStatus& status){
    int dist = 0;
    int i;
    if(!top_border){
        i = 0;
        while(pattern[i] == 0){
            i++;
        }
        dist += (pos2k(i, status));
    }
    if(!bottom_border){
        i = pat_length-1;
        while(pattern[i] == 0){
            i--;
        }
        dist += (status.w - pos2k(i, status) - 1);
    }
    return dist;
}

int BoundaryPattern::pos2k(int pos, RectStatus &status) {
    return (pos * 2) - status.big_col + (pos < status.k_pos);
}



BoundaryGraph::BoundaryGraph(int max_n){
    this->max_n = max_n;
    num_of_nodes = 0;
    node_degree = (int*)malloc(max_n*sizeof(int));
    adj_mat = (Edge*) malloc(max_n*max_n*sizeof(Edge));
}


BoundaryGraph::~BoundaryGraph(){
    free(node_degree);
    free(adj_mat);
}



void BoundaryGraph::insert(int s, int t, int d){
    insert_directed(s, t, d);
    insert_directed(t, s, d);
}

void BoundaryGraph::insert_directed(int s, int t, int d) {
    // Assuming nodes are added in sequential increasing order, it is indeed the case.
    if(s >= num_of_nodes){
        node_degree[s] = 0;
        num_of_nodes++;
    }
    adj_mat[s*max_n+node_degree[s]] = {t, d};
    node_degree[s]++;
}



bool Edge::operator<(const Edge &e) {
    return this->weight < e.weight;
}


bool Edge::operator>(const Edge &e) {
    return this->weight > e.weight;
}

pair<int, int> BoundaryGraph::get_mst_weight() {
    int total_weight = 0, max_weight = 0;
    int pos;
    bool visited[num_of_nodes];
    int num_of_edges = 0;
    for (int i = 0; i < num_of_nodes; ++i) {
        visited[i] = false;
        num_of_edges += node_degree[i];
    }
    Edge cur_edge;
    priority_queue<Edge, vector<Edge>, greater<>> edge_q;
    edge_q.push({0,0});
    while(!edge_q.empty()){
        cur_edge = edge_q.top();
        edge_q.pop();
        if(!visited[cur_edge.target]){
            visited[cur_edge.target] = true;
            total_weight += cur_edge.weight;
            max_weight = max(max_weight, cur_edge.weight);
            pos = cur_edge.target*max_n;
            for (int i = 0; i < node_degree[cur_edge.target]; ++i) {
                if (!visited[adj_mat[pos].target]) {
                    edge_q.push(adj_mat[pos]);
                }
                pos++;
            }
        }
    }
    return {total_weight, max_weight}; //todo
}