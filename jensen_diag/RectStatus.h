//
// Created by gilbe on 22.2.2022.
//

#ifndef JENSEN_DIAG_RECTSTATUS_H
#define JENSEN_DIAG_RECTSTATUS_H


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


#endif //JENSEN_DIAG_RECTSTATUS_H
