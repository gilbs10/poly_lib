cmake_minimum_required(VERSION 3.6)
project(jensen_diag)

set( CMAKE_VERBOSE_MAKEFILE on )

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS -fopenmp)
set(CMAKE_CXX_FLAGS_RELEASE "-fomit-frame-pointer -flto -O3 -march=native")
add_subdirectory(abseil-cpp)

set(SOURCE_FILES main.cpp data_types.cpp data_types.h genfunc.cpp BoundryPattern.cpp BoundryPattern.h SigDict.cpp SigDict.h RectManager.cpp RectManager.h robin_hood.h settings.h RectStatus.cpp RectStatus.h PackedArray.cpp PackedArray.h utils.h utils.cpp)
message("-- Target compiler flags are: ${CXX_F}")
add_executable(jensen_diag ${SOURCE_FILES})


# Declare dependency on the absl::strings library
target_link_libraries(jensen_diag absl::flat_hash_map)