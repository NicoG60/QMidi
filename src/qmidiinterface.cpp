#include <qmidiinterface.h>
#include <qmidiinterface_p.h>

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

void QMidiInterface::swap(QMidiInterface& other) noexcept
{
    qSwap(d, other.d);
}

bool QMidiInterface::isValid() const
{
    return d->index != -1;
}

int QMidiInterface::index() const
{
    return d->index;
}

QMidiInterface::Api QMidiInterface::api() const
{
    return d->api;
}

QString QMidiInterface::name() const
{
    return d->name;
}
