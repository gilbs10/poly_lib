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
sig BoundaryPattern::get_sig_num(){
    sig sig_num = 0;
    for (int i = pat_length-1; i >= 0; --i) {
        sig_num *= 5;
        sig_num += pattern[i];
    }
    return sig_num*4 + 2*top_border + bottom_border;
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