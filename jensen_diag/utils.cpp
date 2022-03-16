//
// Created by gilbe on 14.3.2022.
//

#include "utils.h"
#include "omp.h"
#include <filesystem>
#include <iostream>

string get_time(){
    auto timenow =
            chrono::system_clock::to_time_t(chrono::system_clock::now());
    std::string t( ctime( &timenow ) );
    return t.substr( 0, t.length() -1  ) + " ";
}

filesystem::path get_available_fname(filesystem::path path, string extention){
    int thread_num = omp_get_thread_num();
    int i = 0;
    filesystem::path fname;
    while(true){
        fname = path;
        fname.append(to_string(thread_num)+ "_" + to_string(i) + "." + extention);
        if(filesystem::exists(fname)){
            i++;
        }
        else{
            return fname;
        }
    }
}

float mem_usage(){
    return 0; //todo
}