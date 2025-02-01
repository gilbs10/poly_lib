//
// Created by gilbe on 22.2.2022.
//

#include "RectStatus.h"

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