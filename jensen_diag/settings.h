//
// Created by gilbe on 17.2.2022.
//

#ifndef JENSEN_DIAG_SETTINGS_H
#define JENSEN_DIAG_SETTINGS_H

#include <filesystem>

using namespace std;

/*******************************************************
 *                     GenFunc                         *
 *******************************************************/
//The GenFunc class is the bottleneck of the program, thus we modify the code using defines to make no redundant code
//Only use packing if not using preallocated array
//#define GF_USE_PREALLOCATED
//#define GF_CONST_SIZE 31
//#define GF_MOD_CONST 4611686018427387903
//#define GF_MOD_CONST 36028797018963968
#define GF_MOD_CONST 281474976710655
//#define GF_MOD_CONST 255
#define GF_USE_PACKING


/*******************************************************
 *                  BoundryPattern                     *
 *******************************************************/
//#define LIMIT_SIG_64 //Use only when w<52, it uses 64 bit signatures instead of 128 bit. Faster and less memory.
const bool USE_REVERSING = true;


/*******************************************************
 *                      SigDict                        *
 *******************************************************/
const bool SD_USE_PACKING = true;
#define SD_PACK_TO_FILE
const double SD_PERSISTANT_THRESHOLD = 0.01;
const bool SD_USE_PERSISTANT = false;

/*******************************************************
 *                    PackedArray                      *
 *******************************************************/
const int PREENTRY_BITS=8;
const int INDEX_BITS=7; // 6 For n<=63, 7 for higher. I wish 8 was needed.
const int NUM_OF_SIGS_BITS = 32;
const filesystem::path PAS_SWAP_PATH = "/home/gilbe/temp/packed_array_swap/";
const string PAS_SWAP_EXT = "paf";
const int PAS_BUFFER_SIZE = 512;



/*******************************************************
 *                    RectManager                      *
 *******************************************************/
#define USE_PARALLEL
const bool FIXED_NUM_OF_THREADS = true;
const int RM_REDIST_FREQ = 4;

/*******************************************************
 *                      Printing                       *
 *******************************************************/
const bool PRINT_RES_BY_COL = true;

#endif //JENSEN_DIAG_SETTINGS_H
