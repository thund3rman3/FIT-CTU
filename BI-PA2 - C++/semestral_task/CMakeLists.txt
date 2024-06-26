cmake_minimum_required(VERSION 3.20)
project(VelkaUloha VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -pedantic -g)
endif()
add_executable(VelkaUloha test.cpp)

find_library(EXPRESSION_PARSER_LIB expression_parser  PATHS ./i686-w64-mingw32)

target_link_libraries(VelkaUloha ${EXPRESSION_PARSER_LIB})