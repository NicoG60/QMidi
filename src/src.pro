QT = core core_private

TEMPLATE = lib
DEFINES += QMIDI_LIBRARY

CONFIG += c++11

TARGET = QMidi

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

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
