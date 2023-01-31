cmake_minimum_required(VERSION 3.24)
project(8bit_emulator)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(src)

add_executable(8bit_emulator
        src/main.cpp)
