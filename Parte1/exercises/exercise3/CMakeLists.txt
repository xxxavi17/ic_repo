cmake_minimum_required(VERSION 3.20)

project(exercise3)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED sndfile)

# Include directories
include_directories(${SNDFILE_INCLUDE_DIRS})

# Set output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Define the main executable
add_executable(wav_cmp src/wav_cmp.cpp)

# Link libraries
target_link_libraries(wav_cmp ${SNDFILE_LIBRARIES})

# Set compile flags
target_compile_options(wav_cmp PRIVATE ${SNDFILE_CFLAGS_OTHER})

# Optional: Add compiler warnings
target_compile_options(wav_cmp PRIVATE -Wall -Wextra -Wpedantic)

# Debug and Release configurations
set(CMAKE_BUILD_TYPE Release)
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(wav_cmp PRIVATE -g -O0)
else()
    target_compile_options(wav_cmp PRIVATE -O3)
endif()