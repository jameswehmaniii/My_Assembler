cmake_minimum_required(VERSION 3.26)
project(Assembler_2 C)

set(CMAKE_C_STANDARD 23)

add_executable(Assembler_2 main.c
        opcodes.c
        directives.c
        errors.c
        symbols.c
        headers.h)
