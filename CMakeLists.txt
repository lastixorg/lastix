cmake_minimum_required(VERSION 3.28)

project(
    lastix
    DESCRIPTION ""
    LANGUAGES CXX
    VERSION 0.1.0
)

if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

option(LASTIX_BUILD_EXAMPLES "Build examples" ON)
option(LASTIX_BUILD_TESTS "Build tests" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

include("cmake/lastix.cmake")
include("cmake/CPM.cmake")

add_subdirectory("core/")

if(LASTIX_BUILD_EXAMPLES)
    add_subdirectory("examples/")
endif()

if (LASTIX_BUILD_TESTS)
    include(CTest)
    add_subdirectory("tests/")
endif()
