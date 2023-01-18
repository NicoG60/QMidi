# set headers
set(header_path "${QMIDI_SOURCE_DIR}/include")
file(GLOB headers ${header_path}/QM* ${header_path}/qm*)

# set sources
file(GLOB src *.cpp)

# define library target
add_library(QMidi ${headers} ${src})
target_link_libraries(QMidi PUBLIC Qt::Core)
target_link_libraries(QMidi PUBLIC rtmidi)
target_include_directories(
    QMidi
    PUBLIC
        $<BUILD_INTERFACE:${header_path}>
        $<INSTALL_INTERFACE:${include_dest}>
)
target_compile_definitions(QMidi PRIVATE QMIDI_LIBRARY)
target_compile_definitions(QMidi PRIVATE SOFT_VERSION=${PROJECT_VERSION})
target_compile_definitions(QMidi PRIVATE GIT_VERSION=${GIT_VERSION})

#if(APPLE)
#    target_link_libraries(QMidi PUBLIC "-framework CoreMIDI" "-framework CoreAudio" "-framework CoreFoundation")
#    target_compile_definitions(QMidi PRIVATE __MACOSX_CORE__)
#elseif(WIN32)
#    target_link_libraries(QMidi PUBLIC winmm)
#    target_compile_definitions(QMidi PRIVATE __WINDOWS_MM__)
#elseif(UNIX)
#    include(FindALSA)
#    find_package(Threads REQUIRED CMAKE_THREAD_PREFER_PTHREAD THREADS_PREFER_PTHREAD_FLAG)
#    target_link_libraries(QMidi PUBLIC ALSA::ALSA Threads::Threads)
#    target_compile_definitions(QMidi PRIVATE __LINUX_ALSA__)
#endif()


# install
install(TARGETS QMidi EXPORT QMidi DESTINATION ${lib_dest})
install(FILES ${headers} DESTINATION ${include_dest})
