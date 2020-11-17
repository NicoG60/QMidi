#include <qmidi.h>

#include "qmidi_p.h"
#include "qmidiinterface_p.h"

#include <QDebug>

QMidiPrivate::QMidiPrivate() :
    QObjectPrivate()
{

}

void QMidiPrivate::init(QMidi::Api api, QString clientName)
{
    try
    {
        if(clientName.isEmpty())
            clientName = "QMidi Client";

        midiIn.reset(new RtMidiIn(static_cast<RtMidi::Api>(api), clientName.toStdString()));
        midiOut.reset(new RtMidiOut(static_cast<RtMidi::Api>(api), clientName.toStdString()));

        this->hasError = false;
        this->api = static_cast<QMidi::Api>(midiIn->getCurrentApi());
        this->clientName = clientName;
        this->openedDir = QMidi::UnknownDirection;
        this->error = QMidi::UnspecifiedError;
    }
    catch(RtMidiError& e)
    {
        this->hasError = true;
        this->error = static_cast<QMidi::MidiError>(e.getType());
        this->errorString = QString::fromStdString(e.getMessage());
        qWarning() << "QMidi:" << errorString;
    }
}

QMidiInterface QMidiPrivate::interface(RtMidi& dev, QMidi::Direction dir, unsigned int port)
{
    if(hasError)
    {
        qWarning() << "QMidi: Query for interface while error occured";
        return {};
    }

    QMidiInterface i;
    i.d->api       = api;
    i.d->direction = dir;
    i.d->name      = QString::fromStdString(dev.getPortName(port));
    i.d->index     = port;

    return i;
}

QMidiInterface QMidiPrivate::makeVirtual(QString name)
{
    if(name.isEmpty())
        name = "QMidi Virtual Port";

    QMidiInterface i;
    i.d->api       = api;
    i.d->direction = QMidi::Input | QMidi::Output;
    i.d->name      = name;
    i.d->index     = -2;

    return i;
}

void QMidiPrivate::send(const QByteArray& msg)
{
    if(!openedDir.testFlag(QMidi::Output))
    {
        qWarning() << "QMidi: try to send data with input only object. No effect";
        return;
    }

    midiOut->sendMessage(reinterpret_cast<const unsigned char*>(msg.data()), msg.size());
}

void QMidiPrivate::send(quint8 status, quint8 chan, quint8 data1)
{
    static std::vector<unsigned char> v(2, 0);

    if(!openedDir.testFlag(QMidi::Output))
    {
        qWarning() << "QMidi: try to send data with input only object. No effect";
        return;
    }

    if(chan > 16)
    {
        qWarning() <<  "QMidi: Send a message to a channel over 16. No effect.";
        return;
    }

    if(data1 > 0x7F)
    {
        qWarning() <<  "QMidi: data1 i too large. No effect";
        return;
    }

    v[0] = status | chan;
    v[1] = data1;

    midiOut->sendMessage(&v);
}

void QMidiPrivate::send(quint8 status, quint8 chan, quint8 data1, quint8 data2)
{
    static std::vector<unsigned char> v(3, 0);

    if(!openedDir.testFlag(QMidi::Output))
    {
        qWarning() << "QMidi: try to send data with input only object. No effect";
        return;
    }

    if(chan > 16)
    {
        qWarning() <<  "QMidi: Send a message to a channel over 16. No effect.";
        return;
    }

    if(data1 > 0x7F)
    {
        qWarning() <<  "QMidi: data1 i too large. No effect";
        return;
    }

    if(data2 > 0x7F)
    {
        qWarning() <<  "QMidi: data2 i too large. No effect";
        return;
    }

    v[0] = status | chan;
    v[1] = data1;
    v[2] = data2;

    midiOut->sendMessage(&v);
}



// =============================================================================




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
        r << d->interface(*d->midiIn, Input, i);

    return r;
}

QList<QMidiInterface> QMidi::availableOutputInterfaces()
{
    Q_D(QMidi);

    QList<QMidiInterface> r;

    unsigned int count = d->midiOut->getPortCount();
    for(unsigned int i = 0; i < count; i++)
        r << d->interface(*d->midiOut, Output, i);

    return r;
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
    switch(api)
    {
    case UnspecifiedApi: return QStringLiteral("Unspecified");
    case CoreMidi:       return QStringLiteral("Apple Core Midi");
    case ALSA:           return QStringLiteral("ALSA");
    case JACK:           return QStringLiteral("JACK");
    case WindowsMM:      return QStringLiteral("Windows Multimedia Library");
    default:             return QStringLiteral("");
    }
}

QString QMidi::errorToString(MidiError err)
{
    switch(err)
    {
    case Warning:          return QStringLiteral("Warning");
    case DebugWarning:     return QStringLiteral("Debug");
    case UnspecifiedError: return QStringLiteral("Unspecified Error");
    case NoDevicesFound:   return QStringLiteral("No Device Found");
    case InvalidDevice:    return QStringLiteral("Invalid Device");
    case MemoryError:      return QStringLiteral("Memory Error");
    case InvalidParameter: return QStringLiteral("Invalid Parameter");
    case InvalidUse:       return QStringLiteral("Invalid Use");
    case DriverError:      return QStringLiteral("Driver Error");
    case SystemError:      return QStringLiteral("System Error");
    case ThreadError:      return QStringLiteral("Thread Error");
    default:               return QStringLiteral("");
    }
}

void QMidi::sendMessage(const QByteArray& msg)
{
    d_func()->send(msg);
}

void QMidi::sendNoteOff(const quint8 chan, const quint8 note, const quint8 vel)
{
    d_func()->send(MIDI_STATUS_NOTEOFF, chan, note, vel);
}

void QMidi::sendNoteOn(const quint8 chan, const quint8 note, const quint8 vel)
{
    d_func()->send(MIDI_STATUS_NOTEON, chan, note, vel);
}

void QMidi::sendKeyPressure(const quint8 chan, const quint8 note, const quint8 vel)
{
    d_func()->send(MIDI_STATUS_KEYPRESURE, chan, note, vel);
}

void QMidi::sendControler(const quint8 chan, const quint8 control, const quint8 value)
{
    d_func()->send(MIDI_STATUS_CONTROLCHANGE, chan, control, value);
}

void QMidi::sendProgram(const quint8 chan, const quint8 program)
{
    d_func()->send(MIDI_STATUS_PROGRAMCHANGE, chan, program);
}

void QMidi::sendChannelPressure(const quint8 chan, const quint8 value)
{
    d_func()->send(MIDI_STATUS_CHANNELPRESSURE, chan, value);
}

void QMidi::sendPitchBend(const quint8 chan, const qint16 value)
{
    d_func()->send(MIDI_STATUS_PITCHBEND, chan, value & 0xFF, (value >> 8) & 0xFF);
}
