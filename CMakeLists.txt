cmake_minimum_required(VERSION 3.15)
project(chip8)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

add_subdirectory(SDL EXCLUDE_FROM_ALL)

include_directories(inc)

file(GLOB src "src/*.c")

if(IMGUI_DEBUG)
include_directories(imgui)
include_directories(imgui/backends)

file(GLOB src_cpp "imgui/*.cpp")
file(GLOB src_rend "imgui/backends/*.cpp")

add_executable(chip8 src/imgui_main.cpp ${src_cpp} ${src_rend} ${src})
else()
add_executable(chip8 ${src})
endif()

target_compile_options(chip8 PRIVATE -g -Wall -Werror)

target_link_libraries(chip8 PRIVATE SDL3::SDL3)

