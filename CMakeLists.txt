cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(QMidi VERSION 1.1)
set(QMIDI_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

cmake_policy(SET CMP0077 NEW)

execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE GIT_VERSION
)

execute_process(COMMAND git submodule update --init third_party/rtmidi
                WORKING_DIRECTORY ${QMIDI_SOURCE_DIR})

#Configure the project
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

if(MSVC)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

option(QMIDI_BUILD_TESTS "Build the unit tests"  OFF)

#install var
set(include_dest "include/QMidi-${QMidi_VERSION}")
set(lib_dest "lib/QMidi-${QMidi_VERSION}")

# setup deps
if(NOT TARGET Qt::Core)
    find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
    set(Qt "Qt${QT_VERSION_MAJOR}")
    find_package(${Qt} COMPONENTS Core REQUIRED)
endif()

if(${QMIDI_BUILD_TESTS})
    find_package(${Qt} COMPONENTS Test REQUIRED)
endif()

# add subdirs
add_subdirectory(third_party)
add_subdirectory(src)

if(${QMIDI_BUILD_TESTS})
    enable_testing()
    add_subdirectory(tests)
endif()

# install
install(FILES QMidi-config.cmake DESTINATION ${lib_dest})
install(EXPORT QMidi DESTINATION ${lib_dest})
