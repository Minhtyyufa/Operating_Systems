cmake_minimum_required(VERSION 3.12)
project(hw6)

set(CMAKE_CXX_STANDARD 14)

add_executable(hw6 spin_lock.c spin_lock.h spin_test.c slab.c slab.h dll.c dll.h)