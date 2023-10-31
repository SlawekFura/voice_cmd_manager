include(${CMAKE_SOURCE_DIR}/cmake_utils/FindDBus.cmake)
include(${CMAKE_SOURCE_DIR}/cmake_utils/FindSndFile.cmake)

# Use results of find_package() call.
include_directories(${DBUS_INCLUDE_DIRS})

find_library(sndfile REQUIRED)

include("/usr/share/cmake-3.13/Modules/FindBoost.cmake")
FIND_PACKAGE( Boost 1.40 COMPONENTS program_options filesystem REQUIRED )

include("/usr/share/cmake-3.13/Modules/FindPackageHandleStandardArgs.cmake")
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ALSA DEFAULT_MSG ALSA_LIBRARY ALSA_INCLUDE_DIR)
find_path(ALSA_INCLUDE_DIR NAMES asound.h PATH_SUFFIXES alsa DOC "The ALSA (asound) include directory")
find_library(ALSA_LIBRARY NAMES asound DOC "The ALSA (asound) library")

message("SNDFILE_LIBRARY:" ${SNDFILE_LIBRARY})
message("SNDFILE_INCLUDE_DIR:" ${SNDFILE_INCLUDE_DIR})
message("Boost_LIBRARIES:" ${Boost_LIBRARIES})
message("Boost_INCLUDE_DIR:" ${Boost_INCLUDE_DIR})
message("ALSA_INCLUDE_DIR:" ${ALSA_INCLUDE_DIR})
message("ALSA_LIBRARY:" ${ALSA_LIBRARY})


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules/dbus)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules/intents)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/fsm/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/modules/audio)
include_directories(${Boost_INCLUDE_DIR})
include_directories(${SNDFILE_INCLUDE_DIR})
include_directories(${ALSA_INCLUDE_DIR})

add_executable(main
               main.cpp
               fsm/major_fsm.cpp
               common/types.cpp
               modules/dbus/dbus_server.cpp
               modules/intents/intent_manager.cpp
               modules/audio/audio_mgr.cpp)

target_link_libraries(main ${DBUS_LIBRARIES})
target_link_libraries(main /usr/local/lib/libportaudio.a)
# target_link_libraries(main /usr/lib/arm-linux-gnueabihf/libsndfile.a)
target_link_libraries(main ${SNDFILE_LIBRARY})
target_link_libraries(main ${ALSA_LIBRARY})
target_link_libraries(main ${Boost_LIBRARIES} )
target_link_libraries(main stdc++fs)
