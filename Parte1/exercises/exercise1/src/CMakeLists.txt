cmake_minimum_required(VERSION 3.16)
project(exercise1-wav-hist-extended)

SET (CMAKE_BUILD_TYPE "Release")
#SET (CMAKE_BUILD_TYPE "Debug")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++20")
SET (CMAKE_CXX_FLAGS_RELEASE "-O3")
SET (CMAKE_CXX_FLAGS_DEBUG "-g3 -fsanitize=address")

SET (BASE_DIR ${CMAKE_SOURCE_DIR} )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/../bin)

# Create bin directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable (wav_hist_extended wav_hist_extended.cpp)
target_link_libraries (wav_hist_extended sndfile)

add_executable (channel_recovery_demo channel_recovery_demo.cpp)
target_link_libraries (channel_recovery_demo sndfile)