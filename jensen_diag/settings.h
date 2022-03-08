//
// Created by gilbe on 17.2.2022.
//

#ifndef JENSEN_DIAG_SETTINGS_H
#define JENSEN_DIAG_SETTINGS_H
#include <iomanip>
#include <chrono>

using namespace std;

//The GenFunc class is the bottleneck of the program, thus we modify the code using defines to make no redundant code
//Only use packing if not using preallocated array
#define GF_USE_PREALLOCATED
#define GF_CONST_SIZE 31
//#define GF_USE_PACKING
//#define GF_MOD_CONST 4611686018427387903
//#define GF_MOD_CONST 36028797018963968
#define GF_MOD_CONST 2147483648
//#define GF_MOD_CONST 65535
//#define LIMIT_SIG_64

const bool PRINT_RES_BY_COL = true;
const bool FIXED_NUM_OF_THREADS = true;

string get_time();

#define ALIGN_TAB "\t"
#define FORMAT_ATTR(a,b) " " << (a) <<": "<< (b) << ALIGN_TAB
#define FORMAT_TITLE(a) get_time() << " <" << (a) << ">" << ALIGN_TAB


#endif //JENSEN_DIAG_SETTINGS_H
