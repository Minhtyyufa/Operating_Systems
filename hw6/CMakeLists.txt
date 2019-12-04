cmake_minimum_required(VERSION 3.12)
project(hw6)

set(CMAKE_CXX_STANDARD 14)
enable_language(C ASM)
add_executable(hw6 slab_test.c spin_lock.c spin_lock.h tas64.S tas.h slab.c slab.h dll.c dll.h both.h)