add_executable(midi tst_midi.cpp)
target_link_libraries(midi PUBLIC QMidi Qt5::Test)
add_test(NAME midi COMMAND midi)

