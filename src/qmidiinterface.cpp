#include <qmidiinterface.h>
#include "qmidiinterface_p.h"
#include <QDebug>

QMidiInterface::QMidiInterface() :
    d(new QMidiInterfacePrivate)
{}

QMidiInterface::QMidiInterface(const QMidiInterface& copy) :
    QMidiInterface()
{
    *d = *copy.d;
}

QMidiInterface::QMidiInterface(QMidiInterface&& move)
{
    swap(move);
}

QMidiInterface& QMidiInterface::operator=(const QMidiInterface& copy)
{
    *d = *copy.d;
    return *this;
}

QMidiInterface& QMidiInterface::operator=(QMidiInterface&& move)
{
    swap(move);
    return *this;
}

QMidiInterface::~QMidiInterface() {}

bool QMidiInterface::operator==(const QMidiInterface& other) const
{
    return api() == other.api() &&
            index() == other.index() &&
            directions() == other.directions() &&
            name() == other.name();
}

bool QMidiInterface::operator!=(const QMidiInterface& other) const
{
    return !(*this == other);
}

void QMidiInterface::swap(QMidiInterface& other) noexcept
{
    qSwap(d, other.d);
}

bool QMidiInterface::isValid() const
{
    return d->index != -1;
}

bool QMidiInterface::isVirtual() const
{
    return d->index < -1;
}

int QMidiInterface::index() const
{
    return d->index;
}

QMidi::Api QMidiInterface::api() const
{
    return d->api;
}

QString QMidiInterface::name() const
{
    return d->name;
}

QMidi::Directions QMidiInterface::directions() const
{
    return d->direction;
}

QDebug operator<<(QDebug debug, const QMidiInterface& iface)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "QMidiInterface(" << iface.name() << ", " << iface.directions() << ")";
    return debug;
}
