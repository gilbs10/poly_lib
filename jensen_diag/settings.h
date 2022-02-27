//
// Created by gilbe on 17.2.2022.
//

#ifndef JENSEN_DIAG_SETTINGS_H
#define JENSEN_DIAG_SETTINGS_H

//The GenFunc class is the bottleneck of the program, thus we modify the code using defines to make no redundant code
//Only use packing if not using preallocated array
//#define GF_USE_PREALLOCATED
//#define GF_CONST_SIZE 50
#define GF_USE_PACKING
//#define GF_MOD_CONST 4611686018427387903
//#define GF_MOD_CONST 36028797018963968
#define GF_MOD_CONST 2147483648
//#define GF_MOD_CONST 65535

const bool PRINT_RES_BY_COL = true;



#endif //JENSEN_DIAG_SETTINGS_H
