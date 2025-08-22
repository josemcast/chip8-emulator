cmake_minimum_required(VERSION 3.15)
project(chip8 C)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(SDL EXCLUDE_FROM_ALL)

include_directories(inc)

file(GLOB src "src/*.c")

add_executable(chip8 ${src})

target_compile_options(chip8 PRIVATE -g -Wall -Werror)

target_link_libraries(chip8 PRIVATE SDL3::SDL3)

