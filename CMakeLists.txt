cmake_minimum_required(VERSION 3.15)
project(chip8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(SDL EXCLUDE_FROM_ALL)

include_directories(
    inc 
    lua/src)

file(GLOB src "src/*.c")

add_executable(chip8 main.c ${src})

target_compile_options(chip8 PRIVATE -g -Wall -Werror)

target_link_directories(chip8 PRIVATE lua/src)

target_link_libraries(chip8 PRIVATE 
    SDL3::SDL3
    lua
    m)
