#ifndef QMIDIINTERFACE_H
#define QMIDIINTERFACE_H

#include "qmidi_global.h"
#include "qmidi.h"

#include <QObject>
#include <QSharedDataPointer>

class QMidiInterfacePrivate;
class QMIDI_EXPORT QMidiInterface
{
public:
    QMidiInterface();
    QMidiInterface(const QMidiInterface& copy);
    QMidiInterface(QMidiInterface&& move);
    QMidiInterface& operator=(const QMidiInterface& copy);
    QMidiInterface& operator=(QMidiInterface&& move);
    ~QMidiInterface();

    bool operator==(const QMidiInterface& other) const;
    bool operator!=(const QMidiInterface& other) const;

    void swap(QMidiInterface& other) noexcept;

    bool isValid() const;

    bool isVirtual() const;

    int        index() const;
    QMidi::Api api()  const;
    QString    name() const;
    QMidi::Directions directions() const;

    friend QDebug operator<<(QDebug debug, const QMidiInterface& iface);

private:
    friend class QMidi;
    friend class QMidiPrivate;
    QScopedPointer<QMidiInterfacePrivate> d;
};

#endif // QMIDIINTERFACE_H
