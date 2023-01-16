set(RTMIDI_BUILD_STATIC_LIBS ON)
set(RTMIDI_BUILD_TESTING OFF)
add_subdirectory(rtmidi EXCLUDE_FROM_ALL)
target_compile_definitions(rtmidi PUBLIC TARGET_OS_IPHONE=0)

add_library(qmidi_third_party INTERFACE)
target_link_libraries(
    qmidi_third_party INTERFACE
        rtmidi
)
