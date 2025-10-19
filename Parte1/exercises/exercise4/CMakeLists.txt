cmake_minimum_required(VERSION 3.16)

project(wav_effects VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Default to Release build
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# Find required packages
find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED sndfile)

# Include directories
include_directories(${SNDFILE_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/AudioEffect.cpp
    src/AudioEffectFactory.cpp
    src/EchoEffect.cpp
    src/MultiEchoEffect.cpp
    src/AmplitudeModulationEffect.cpp
    src/ChorusEffect.cpp
    src/ReverbEffect.cpp
    src/wav_effects.cpp
)

# Create executable
add_executable(wav_effects ${SOURCES})

# Link libraries
target_link_libraries(wav_effects ${SNDFILE_LIBRARIES})
target_link_directories(wav_effects PRIVATE ${SNDFILE_LIBRARY_DIRS})
target_compile_options(wav_effects PRIVATE ${SNDFILE_CFLAGS_OTHER})

# Set output directory
set_target_properties(wav_effects PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Installation
install(TARGETS wav_effects
    RUNTIME DESTINATION bin
)

# Print configuration info
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "libsndfile found: ${SNDFILE_FOUND}")
message(STATUS "libsndfile version: ${SNDFILE_VERSION}")

# Create directories
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/results)