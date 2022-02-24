cmake_minimum_required(VERSION 3.6)
project(jensen_diag)

set( CMAKE_VERBOSE_MAKEFILE on )

if (CMAKE_BUILD_TYPE MATCHES Debug)
    message(" ")
    message("CMAKE IN DEBUG MODE")
    message(" ")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message(" ")
    message("CMAKE IN RELEASE MODE")
    message(" ")
endif ()

set(CMAKE_CXX_STANDARD 17)

SET(CMAKE_CXX_FLAGS -fopenmp)
set(CMAKE_CXX_FLAGS_RELEASE "-fprofile-use -fprofile-correction -O3 -march=native")
add_subdirectory(abseil-cpp)

set(SOURCE_FILES main.cpp genfunc.cpp BoundryPattern.cpp BoundryPattern.h SigDict.cpp SigDict.h RectManager.cpp RectManager.h robin_hood.h settings.h RectStatus.cpp RectStatus.h)
message("-- Target compiler flags are: ${CXX_F}")
add_executable(jensen_diag ${SOURCE_FILES})


# Declare dependency on the absl::strings library
target_link_libraries(jensen_diag absl::flat_hash_map)