cmake_minimum_required(VERSION 3.13)
project(intent_processing)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

set(COMMON_LIBS dl)
set(MIC_LIBS pthread m)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/)

add_subdirectory(${CMAKE_SOURCE_DIR}/sources)
