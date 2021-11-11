cmake_minimum_required(VERSION 3.7)

project(bpf_tracer VERSION 1.0.0 DESCRIPTION "Memset benchmarks")

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

add_compile_options(-Wall -g3 -O3 -march=native)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -march=native")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -fno-omit-frame-pointer -O0")

# Place all of the binaries in the build directory.
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(BEFORE
  ${CMAKE_CURRENT_BINARY_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  )

add_library(mem_shim SHARED
            src/shims.c
            src/calls.s
            src/lib.c
           )

set_target_properties(mem_shim PROPERTIES
     VERSION ${PROJECT_VERSION}
     SOVERSION 1
     )

add_library(mem_hist SHARED
            src/memset_hist.c
           )

set_target_properties(mem_hist PROPERTIES
     VERSION ${PROJECT_VERSION}
     SOVERSION 1
     )

add_executable(bench_memset
                 src/main.cc
                 src/calls.s
                 src/lib.c
                 )

add_executable(test_memset
                 src/test.cc
                 src/calls.s
                 src/lib.c
                 )

target_link_libraries(bench_memset PUBLIC)
target_link_libraries(test_memset PUBLIC)

install(TARGETS bench_memset DESTINATION bin)
install(TARGETS test_memset DESTINATION bin)
install(TARGETS mem_shim LIBRARY DESTINATION bin)

