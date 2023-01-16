#ifndef QMIDIINTERFACE_P_H
#define QMIDIINTERFACE_P_H

#include <qmidiinterface.h>

class QMidiInterfacePrivate
{
    Q_DECLARE_PUBLIC(QMidiInterface)

public:
    QMidiInterfacePrivate() = default;
    QMidiInterfacePrivate(const QMidiInterfacePrivate&) = default;
    QMidiInterfacePrivate(QMidiInterfacePrivate&&) noexcept = default;
    ~QMidiInterfacePrivate() = default;

    QMidiInterfacePrivate& operator=(const QMidiInterfacePrivate&) = default;
    QMidiInterfacePrivate& operator=(QMidiInterfacePrivate&&) noexcept = default;

    QMidiInterface* q_ptr = nullptr;

    int index = -1;
    QMidiApi api = UnspecifiedApi;
    QMidiDirections direction = UnknownDirection;
    QString name;
};

#endif // QMIDIINTERFACE_P_H
