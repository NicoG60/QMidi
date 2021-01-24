# set headers
set(header_path "${QMIDI_SOURCE_DIR}/include")
file(GLOB headers ${header_path}/QM* ${header_path}/qm*)

# set sources
file(GLOB src *.cpp)

# setup deps
find_package(Qt5 COMPONENTS Core REQUIRED)

find_package(RtMidi QUIET)

if(NOT RtMidi_FOUND)
    execute_process(COMMAND git submodule update --init third_party/rtmidi
                    WORKING_DIRECTORY ${QMIDI_SOURCE_DIR})

    set(RtMidi_DIR ${QMIDI_SOURCE_DIR}/third_party/rtmidi)

    add_library(rtmidi INTERFACE ${RtMidi_DIR}/RtMidi.cpp ${RtMidi_DIR}/RtMidi.h)
    target_include_directories(rtmidi INTERFACE ${RtMidi_DIR})
    target_compile_definitions(rtmidi INTERFACE RTMIDI_EXPORT)

    if(APPLE)
        target_link_libraries(rtmidi INTERFACE "-framework CoreMIDI" "-framework CoreAudio" "-framework CoreFoundation")
        target_compile_definitions(rtmidi INTERFACE __MACOSX_CORE__)
    elseif(WIN32)
        target_link_libraries(rtmidi INTERFACE winmm)
        target_compile_definitions(rtmidi INTERFACE __WINDOWS_MM__)
    elseif(UNIX)
        include(FindALSA)
        find_package(Threads REQUIRED CMAKE_THREAD_PREFER_PTHREAD THREADS_PREFER_PTHREAD_FLAG)
        target_link_libraries(rtmidi INTERFACE ALSA::ALSA Threads::Threads)
        target_compile_definitions(rtmidi INTERFACE __LINUX_ALSA__)
    endif()

    add_library(RtMidi::rtmidi ALIAS rtmidi)
endif()



# define library target
add_library(QMidi ${headers} ${src})
target_link_libraries(QMidi PUBLIC Qt5::Core PRIVATE $<BUILD_INTERFACE:RtMidi::rtmidi>)
target_include_directories(
    QMidi
    PUBLIC
        $<BUILD_INTERFACE:${header_path}>
        $<INSTALL_INTERFACE:${lib_dest}>

    PRIVATE
        ${Qt5Core_PRIVATE_INCLUDE_DIRS}
)
target_compile_definitions(QMidi PRIVATE QMIDI_LIBRARY)

# install
install(TARGETS QMidi EXPORT QMidi DESTINATION ${lib_dest})
install(FILES ${headers} DESTINATION ${include_dest})
