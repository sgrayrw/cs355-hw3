cmake_minimum_required(VERSION 3.14)
project(hw3 C)

set(CMAKE_C_STANDARD 11)

add_executable(hw3
        mysh.c job.h job.c builtin.h builtin.c sighand.h sighand.c)
