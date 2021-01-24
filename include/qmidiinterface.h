#ifndef QMIDIINTERFACE_H
#define QMIDIINTERFACE_H

#include "qmidi_global.h"
#include "qmidi.h"

#include <QObject>
#include <QSharedDataPointer>

class QMidiInterfacePrivate;
class QMIDI_EXPORT QMidiInterface
{
    Q_GADGET

    Q_PROPERTY(int index READ index)
    Q_PROPERTY(QMidi::Api api READ api)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QMidi::Directions directions READ directions)

public:
    QMidiInterface();
    QMidiInterface(const QMidiInterface& copy);
    QMidiInterface(QMidiInterface&& move) noexcept;
    QMidiInterface& operator=(const QMidiInterface& copy);
    QMidiInterface& operator=(QMidiInterface&& move) noexcept;
    ~QMidiInterface();

    bool operator==(const QMidiInterface& other) const;
    bool operator!=(const QMidiInterface& other) const;

    void swap(QMidiInterface& other) noexcept;

    bool isValid() const;

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

Q_DECLARE_METATYPE(QMidiInterface);

#endif // QMIDIINTERFACE_H
