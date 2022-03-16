//
// Created by gilbe on 14.3.2022.
//

#ifndef JENSEN_DIAG_UTILS_H
#define JENSEN_DIAG_UTILS_H


#include <iomanip>
#include <chrono>
#include <filesystem>

using namespace std;

/*******************************************************
 *                   Files & Memory                    *
 *******************************************************/
filesystem::path get_available_fname(filesystem::path path, string extension);
float mem_usage();

/*******************************************************
 *                      Printing                       *
 *******************************************************/
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

#endif //JENSEN_DIAG_UTILS_H
