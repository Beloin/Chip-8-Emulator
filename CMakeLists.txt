cmake_minimum_required(VERSION 3.24)
project(8bit_emulator)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(src)

add_executable(8bit_emulator
        src/main.cpp src/Chip8.h src/Chip8.cpp src/setup/setup.h src/utils/input_utils.cpp src/utils/input_utils.h src/interface/OpenGL.cpp src/interface/OpenGL.h src/setup/setup.cpp)
