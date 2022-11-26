#include <qmidiinterface.h>
#include "qmidiinterface_p.h"
#include <QDebug>

QMidiInterface::QMidiInterface() :
    d_ptr(new QMidiInterfacePrivate)
{
    d_ptr->q_ptr = this;
}

QMidiInterface::QMidiInterface(const QMidiInterface& copy) :
    QMidiInterface()
{
    *d_ptr = *copy.d_ptr;
    d_ptr->q_ptr = this;
}

QMidiInterface::QMidiInterface(QMidiInterface&& move) noexcept
{
    swap(move);
}

QMidiInterface& QMidiInterface::operator=(const QMidiInterface& copy)
{
    *d_ptr = *copy.d_ptr;
    d_ptr->q_ptr = this;
    return *this;
}

QMidiInterface& QMidiInterface::operator=(QMidiInterface&& move) noexcept
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
    qSwap(d_ptr, other.d_ptr);
    qSwap(d_ptr->q_ptr, other.d_ptr->q_ptr);
}

bool QMidiInterface::isValid() const
{
    return d_ptr->index > -1;
}

int QMidiInterface::index() const
{
    return d_ptr->index;
}

QMidi::Api QMidiInterface::api() const
{
    return d_ptr->api;
}

QString QMidiInterface::name() const
{
    return d_ptr->name;
}

QMidi::Directions QMidiInterface::directions() const
{
    return d_ptr->direction;
}

QDebug operator<<(QDebug debug, const QMidiInterface& iface)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "QMidiInterface(" << iface.name() << ", " << iface.directions() << ")";
    return debug;
}
