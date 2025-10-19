cmake_minimum_required(VERSION 3.16)
project(bit_stream)

SET (CMAKE_BUILD_TYPE "Release")
#SET (CMAKE_BUILD_TYPE "Debug")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++20")
SET (CMAKE_CXX_FLAGS_RELEASE "-O3")
SET (CMAKE_CXX_FLAGS_DEBUG "-g3 -fsanitize=address")

SET (BASE_DIR ${CMAKE_SOURCE_DIR} )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/../bin)

# Encontrar biblioteca sndfile
find_package(PkgConfig REQUIRED)
pkg_check_modules(SNDFILE REQUIRED sndfile)

add_library(Common OBJECT)

target_sources(Common PRIVATE bit_stream.cpp byte_stream.cpp)

# Programas originais
add_executable (text2bin text2bin.cpp $<TARGET_OBJECTS:Common>)
add_executable (bin2text bin2text.cpp $<TARGET_OBJECTS:Common>)

# Programas para quantização WAV
add_executable (wav_quant_enc wav_quant_enc.cpp $<TARGET_OBJECTS:Common>)
target_link_libraries(wav_quant_enc ${SNDFILE_LIBRARIES})
target_include_directories(wav_quant_enc PRIVATE ${SNDFILE_INCLUDE_DIRS})

add_executable (wav_quant_dec wav_quant_dec.cpp $<TARGET_OBJECTS:Common>)
target_link_libraries(wav_quant_dec ${SNDFILE_LIBRARIES})
target_include_directories(wav_quant_dec PRIVATE ${SNDFILE_INCLUDE_DIRS})

# Programas para codec DCT
add_executable (wav_dct_enc wav_dct_enc.cpp $<TARGET_OBJECTS:Common>)
target_link_libraries(wav_dct_enc ${SNDFILE_LIBRARIES} m)
target_include_directories(wav_dct_enc PRIVATE ${SNDFILE_INCLUDE_DIRS})

add_executable (wav_dct_dec wav_dct_dec.cpp $<TARGET_OBJECTS:Common>)
target_link_libraries(wav_dct_dec ${SNDFILE_LIBRARIES} m)
target_include_directories(wav_dct_dec PRIVATE ${SNDFILE_INCLUDE_DIRS})

