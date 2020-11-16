#include <qmidi.h>

#include "qmidi_p.h"
#include <QDebug>

QMidi::QMidi(QObject* parent) :
    QObject(*new QMidiPrivate, parent)
{
    d_func()->init();
}

QMidi::QMidi(Api api, QObject* parent) :
    QObject(*new QMidiPrivate, parent)
{
    d_func()->init(api);
}

QMidi::QMidi(Api api, const QString& clientName, QObject* parent) :
    QObject(*new QMidiPrivate, parent)
{
    d_func()->init(api, clientName);
}

QMidi::~QMidi()
{
    if(isOpen())
        close();
}

QMidi::Api QMidi::api() const
{
    return d_func()->api;
}

void QMidi::setApi(Api api)
{
    Q_D(QMidi);

    if(isOpen())
    {
        qWarning() << "QMidi: Setting API on opened device. No effect.";
        return;
    }

    if(api != d->api)
    {
        d->init(api, d->clientName);
        emit apiChanged();
    }
}

QString QMidi::apiName() const
{
    return apiToString(api());
}

bool QMidi::hasError() const
{
    return d_func()->hasError;
}

QMidi::MidiError QMidi::error() const
{
    return d_func()->error;
}

QString QMidi::errorString() const
{
    return d_func()->errorString;
}

QString QMidi::clientName() const
{
    return d_func()->clientName;
}

void QMidi::setClientName(const QString& name)
{
    Q_D(QMidi);

    if(isOpen())
    {
        qWarning() << "QMidi: Setting Client Name on opened device. No effect.";
        return;
    }

    if(name != d->clientName)
    {
        d->init(d->api, name);
        emit clientNameChanged();
    }
}

QMidiInterface QMidi::inputInterface() const
{
    return d_func()->ifaceIn;
}

void QMidi::setInputInterface(const QMidiInterface& i)
{
    Q_D(QMidi);

    if(i.api() != d->ifaceIn.api())
    {
        qWarning() << "QMidi: Setting an interface with different API. No effect.";
        return;
    }

    if(isOpen())
    {
        qWarning() << "QMidi: Setting an interface on opened device. No effect.";
        return;
    }

    if(i.index() != d->ifaceIn.index())
    {
        d->ifaceIn = i;
        emit inputInterfaceChanged();
    }
}

QMidiInterface QMidi::outputInterface() const
{
    return d_func()->ifaceOut;
}

void QMidi::setOutputInterface(const QMidiInterface& i)
{
    Q_D(QMidi);

    if(i.api() != d->ifaceOut.api())
    {
        qWarning() << "QMidi: Setting an interface with different API. No effect.";
        return;
    }

    if(isOpen())
    {
        qWarning() << "QMidi: Setting an interface on opened device. No effect.";
        return;
    }

    if(i.index() != d->ifaceOut.index())
    {
        d->ifaceOut = i;
        emit outputInterfaceChanged();
    }
}

void QMidi::open()
{
    Q_D(QMidi);

    if(isOpen())
    {
        qWarning() << "QMidi: Calling open() on already opened device. No effect.";
        return;
    }

    if(d->ifaceIn.isValid())
    {
        auto idx = d->ifaceIn.index();
        auto name = d->ifaceIn.name().toStdString();
        if(d->ifaceIn.isVirtual())
            d->midiIn->openVirtualPort(name);
        else
            d->midiIn->openPort(idx, name);

        if(!hasError())
            d->openedDir |= Input;
    }

    if(d->ifaceOut.isValid())
    {
        auto idx = d->ifaceOut.index();
        auto name = d->ifaceOut.name().toStdString();
        if(d->ifaceOut.isVirtual())
            d->midiOut->openVirtualPort(name);
        else
            d->midiOut->openPort(idx, name);

        if(!hasError())
            d->openedDir |= Output;
    }

    if(d->openedDir == UnknownDirection)
        qWarning() << "QMidi: Error occured while trying to open midi devices.";
}

bool QMidi::isOpen()
{
    return openedDirection() != UnknownDirection;
}

QMidi::Directions QMidi::openedDirection()
{
    return d_func()->openedDir;
}

void QMidi::close()
{
    Q_D(QMidi);

    if(d->midiIn->isPortOpen())
        d->midiIn->closePort();

    if(d->midiOut->isPortOpen())
        d->midiOut->closePort();

    d->openedDir = UnknownDirection;
}

QList<QMidiInterface> QMidi::availableInputInterfaces()
{
    Q_D(QMidi);

    QList<QMidiInterface> r;

    unsigned int count = d->midiIn->getPortCount();
    for(unsigned int i = 0; i < count; i++)
        r << d->interface(*d->midiIn, i);
}

QList<QMidiInterface> QMidi::availableOutputInterfaces()
{
    Q_D(QMidi);

    QList<QMidiInterface> r;

    unsigned int count = d->midiOut->getPortCount();
    for(unsigned int i = 0; i < count; i++)
        r << d->interface(*d->midiOut, i);
}

QList<QMidiInterface> QMidi::availableInterfaces()
{
    return availableInputInterfaces() + availableOutputInterfaces();
}

QMidiInterface QMidi::createVirtualInterface(const QString& name)
{
    return d_func()->makeVirtual(name);
}

QList<QMidi::Api> QMidi::availableApi()
{
    QList<QMidi::Api> r;
    std::vector<RtMidi::Api> v;

    RtMidi::getCompiledApi(v);

    for(auto a : v)
        r << static_cast<Api>(a);

    return r;
}

QString QMidi::apiToString(Api api)
{

}

QString QMidi::errorToString(MidiError err)
{

}
