//
// Created by gilbe on 17.2.2022.
//

#ifndef JENSEN_DIAG_SETTINGS_H
#define JENSEN_DIAG_SETTINGS_H
#include <iomanip>
#include <chrono>

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
//#define GF_MOD_CONST 2147483648
//#define GF_MOD_CONST 255
#define GF_USE_PACKING


/*******************************************************
 *                  BoundryPattern                     *
 *******************************************************/
#define LIMIT_SIG_64 //Use only when w<52, it uses 64 bit signatures instead of 128 bit. Faster and less memory.


/*******************************************************
 *                      SigDict                        *
 *******************************************************/
const bool SD_USE_PACKING= true;


/*******************************************************
 *                    PackedArray                      *
 *******************************************************/
const int PREENTRY_BITS=7;
const int INDEX_BITS=7; // 6 For n<=63, 7 for higher. I wish 8 was needed.
const int NUM_OF_SIGS_BITS = 64;


/*******************************************************
 *                  Parallelization                    *
 *******************************************************/
#define USE_PARALLEL
const bool FIXED_NUM_OF_THREADS = true;


/*******************************************************
 *                      Printing                       *
 *******************************************************/
const bool PRINT_RES_BY_COL = true;
string get_time();
#define ALIGN_TAB "\t"
#define FORMAT_ATTR(a,b) " " << (a) <<": "<< (b) << ALIGN_TAB
#define FORMAT_TITLE(a) get_time() << " <" << (a) << ">" << ALIGN_TAB
#define LOG_VERBOSE
#ifdef LOG_VERBOSE
#define FORMAT_ATTR_VERBOSE(a,b) " " << (a) <<": "<< (b) << ALIGN_TAB
#define FORMAT_TITLE_VERBOSE(a) get_time() << " <" << (a) << ">" << ALIGN_TAB
#else
#define FORMAT_ATTR_VERBOSE(a,b) ""
#define FORMAT_TITLE_VERBOSE(a) ""
#endif

#endif //JENSEN_DIAG_SETTINGS_H
