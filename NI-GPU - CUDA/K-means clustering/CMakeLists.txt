cmake_minimum_required(VERSION 3.27)
project(KMEANS LANGUAGES CXX)

#project(KMEANS LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 23)
#set(CMAKE_CUDA_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/Zc:__cplusplus /std:c++latest)
    add_compile_options(/W4 /w44265)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(KMEANS main.cpp
)
