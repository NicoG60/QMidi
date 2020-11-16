#ifndef QMIDI_GLOBAL_H
#define QMIDI_GLOBAL_H

#include <QtCore/qtglobal.h>

#if defined(QMIDI_LIBRARY)
#  define QMIDI_EXPORT Q_DECL_EXPORT
#else
#  define QMIDI_EXPORT Q_DECL_IMPORT
#endif

#endif // QMIDI_GLOBAL_H
