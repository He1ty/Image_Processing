cmake_minimum_required(VERSION 3.30)
project(projet_benito C)

set(CMAKE_C_STANDARD 11)

add_executable(projet_benito main.c
        bmp8.h
        bmp8.c
        bmp24.h
        bmp24.c)
