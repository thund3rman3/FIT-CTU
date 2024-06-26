cmake_minimum_required(VERSION 3.10)
project(test)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Source files
set(SOURCES
        solution.cpp
        sample_tester.cpp
)

# Compile flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -O2 -g")

# Executable
add_executable(test ${SOURCES})

find_package(Threads REQUIRED)
target_link_libraries(test PRIVATE
        Threads::Threads
)
target_include_directories(test PRIVATE ${PROJECT_SOURCE_DIR})
target_compile_options(test PRIVATE -Wall -pedantic -g)
# Link progtest_solver and pthread library
target_link_libraries(test PRIVATE ${PROJECT_SOURCE_DIR}/i686-w64-mingw32/libprogtest_solver.a)
