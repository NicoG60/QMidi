QT = core core_private

TEMPLATE = lib
DEFINES += QMIDI_LIBRARY

CONFIG += c++11

TARGET = QMidi

VERSION = 1.0.0
DEFINES += GIT_VERSION=$$system(git rev-parse --short HEAD)
DEFINES += SOFT_VERSION=$$VERSION

SOURCES += \
    $$PWD/qmidi.cpp \
    qmidiinterface.cpp

include($$PWD/../include/include.pri)
include($$PWD/../third_party/third_party.pri)

OBJECTS_DIR = tmp
MOC_DIR = tmp
DESTDIR = lib

HEADERS += \
    qmidi_p.h \
    qmidiinterface_p.h

macx {
    DEFINES += __MACOSX_CORE__
    LIBS+= -framework CoreMIDI -framework CoreAudio -framework CoreFoundation
}
