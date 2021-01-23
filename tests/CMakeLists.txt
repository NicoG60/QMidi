if(${QMIDI_BUILD_TESTS})
    find_package(Qt5 COMPONENTS Test REQUIRED)
    add_subdirectory(midi)
endif()


