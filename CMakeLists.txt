cmake_minimum_required(VERSION 3.23)
project(microbench C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(microbench
        microbench.c
        microbench.h eADR.c)
