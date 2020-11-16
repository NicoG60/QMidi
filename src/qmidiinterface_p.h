#ifndef QMIDIINTERFACE_P_H
#define QMIDIINTERFACE_P_H

#include <qmidiinterface.h>

class QMidiInterfacePrivate
{
public:
    QMidiInterfacePrivate() = default;
    QMidiInterfacePrivate(const QMidiInterfacePrivate&) = default;
    QMidiInterfacePrivate(QMidiInterfacePrivate&) = default;
    ~QMidiInterfacePrivate() = default;

    QMidiInterfacePrivate& operator=(const QMidiInterfacePrivate&) = default;
    QMidiInterfacePrivate& operator=(QMidiInterfacePrivate&&) = default;

    int index = -1;
    QMidi::Api api = QMidi::UnspecifiedApi;
    QMidi::Directions direction = QMidi::UnknownDirection;
    QString name;
};

#endif // QMIDIINTERFACE_P_H
