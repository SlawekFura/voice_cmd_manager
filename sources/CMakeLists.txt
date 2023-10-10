cmake_minimum_required(VERSION 3.13)
project(intent_processing)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

set(COMMON_LIBS dl)
set(MIC_LIBS pthread m)

include(${CMAKE_SOURCE_DIR}/cmake_utils/FindDBus.cmake)

# Use results of find_package() call.
include_directories(${DBUS_INCLUDE_DIRS})

#add_subdirectory(${CMAKE_SOURCE_DIR}/sources)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules/dbus)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules/intents)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/fsm/)

add_executable(main
               main.cpp
               fsm/major_fsm.cpp
               common/types.cpp
               modules/dbus/dbus_server.cpp
               modules/intents/intent_manager.cpp)
               
target_link_libraries(main ${DBUS_LIBRARIES})
