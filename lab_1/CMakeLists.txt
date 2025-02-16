cmake_minimum_required(VERSION 3.10)

project(build_sin_vec)


add_library(Sin_Vec sin_vec.cpp)

add_executable(main main.cpp)


option(USE_FLOAT "Define vec type as float" OFF)

if(USE_FLOAT)
    target_compile_definitions(Sin_Vec PUBLIC USE_FLOAT)
endif()


target_link_libraries(main PUBLIC Sin_Vec)