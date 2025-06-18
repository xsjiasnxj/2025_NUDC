cmake_minimum_required(VERSION 3.22)
project(filterDemo C)

set(CMAKE_C_STANDARD 99)

add_executable(filterDemo main.c filter.c filter.h arm_math.c arm_math.h)
