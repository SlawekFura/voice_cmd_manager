cmake_minimum_required(VERSION 3.13)
project(intent_processing)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

set(COMMON_LIBS dl)
set(MIC_LIBS pthread m)

add_subdirectory(${CMAKE_SOURCE_DIR}/sources)
