cmake_minimum_required(VERSION 3.7)

project(bpf_tracer VERSION 1.0.0 DESCRIPTION "Memset benchmarks")

SET (CMAKE_CXX_COMPILER             "/usr/bin/clang++")

set(CMAKE_CXX_STANDARD 14)
set(CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export a JSON file with the compilation commands that external tools can use
# to analyze the source code of the project.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

enable_language(C ASM)

# Disable exceptions
SET (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-fno-rtti ")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "No build type selected, default to Release")
  set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Build type (default RelWithDebInfo)" FORCE)
endif()

add_compile_options(-Wall -g3 -O3)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -fno-omit-frame-pointer -O0")

# Place all of the binaries in the build directory.
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(BEFORE
  ${CMAKE_CURRENT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )


add_executable(bench_memset
                 src/main.cc
                 src/calls.s
                 src/lib.c
                 )

target_link_libraries(bench_memset
                      PUBLIC
                     )

install(TARGETS bench_memset DESTINATION bin)

