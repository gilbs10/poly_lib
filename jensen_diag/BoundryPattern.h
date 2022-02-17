//
// Created by gilbe on 13.2.2022.
//

#ifndef JENSEN_DIAG_BOUNDRYPATTERN_H
#define JENSEN_DIAG_BOUNDRYPATTERN_H

typedef unsigned long long int sig; // TODO: change to type that will support 128 or arbitrary num of bits


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
    sig get_sig_num();
    sig get_occupancy_num(int s, int t);
    bool sealed();
};




#endif //JENSEN_DIAG_BOUNDRYPATTERN_H
