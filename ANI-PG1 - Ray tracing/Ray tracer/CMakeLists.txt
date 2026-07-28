cmake_minimum_required(VERSION 3.15)
project(RayTracer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE HEADERS "src/*.h")
file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(raytracer ${SOURCES} ${HEADERS})

# Přidání tvých vysněných flagů pro kompilátor
target_compile_options(raytracer PRIVATE -Wall -pedantic -O2 -g)

target_include_directories(raytracer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_include_directories(raytracer PRIVATE ${CMAKE_SOURCE_DIR}/external)

add_custom_target(run
    COMMAND $<TARGET_FILE:raytracer>
    DEPENDS raytracer
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Ray Tracer running..."
)