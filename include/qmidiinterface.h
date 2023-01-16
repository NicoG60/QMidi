#ifndef QMIDIINTERFACE_H
#define QMIDIINTERFACE_H

#include "qmidi_global.h"

#include <QObject>
#include <memory>

class QMidiInterfacePrivate;
class QMIDI_EXPORT QMidiInterface
{
    Q_GADGET

    Q_PROPERTY(int index READ index)
    Q_PROPERTY(QMidiApi api READ api)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QMidiDirections directions READ directions)

    Q_DECLARE_PRIVATE(QMidiInterface);
    friend class QMidiPrivate;

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
    QMidiApi api()  const;
    QString    name() const;
    QMidiDirections directions() const;

    friend QDebug operator<<(QDebug debug, const QMidiInterface& iface);

protected:
    std::unique_ptr<QMidiInterfacePrivate> d_ptr;
};

Q_DECLARE_METATYPE(QMidiInterface);

#endif // QMIDIINTERFACE_H
