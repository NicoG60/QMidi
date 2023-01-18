# Some bullshit tricks to make RtMidi behave as a sub directory
if(NOT DEFINED BUILD_SHARED_LIBS)
    set(BUILD_SHARED_LIBS OFF CACHE BOOL "prevents rtmidi polluting that variable" FORCE)
endif()

set(RTMIDI_BUILD_STATIC_LIBS ON CACHE BOOL "force rtmidi static" FORCE)
set(RTMIDI_BUILD_TESTING OFF CACHE BOOL "disable rtmidi testing" FORCE)
add_subdirectory(rtmidi EXCLUDE_FROM_ALL)
target_compile_definitions(rtmidi PUBLIC TARGET_OS_IPHONE=0)
message(STATUS "shared? ${BUILD_SHARED_LIBS}")
