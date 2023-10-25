cmake_minimum_required(VERSION 3.26)

add_executable(28_4_1
        ../main.cpp
        ../headers/utilities.h
        utilities.cpp
        Swimmer.cpp
        ../headers/Swimmer.h
)

target_include_directories(28_4_1 PUBLIC ${CMAKE_SOURCE_DIR}/headers)