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

        midiIn->setErrorCallback(&QMidiPrivate::errorCallback, this);
        midiOut->setErrorCallback(&QMidiPrivate::errorCallback, this);

        midiIn->setCallback(&QMidiPrivate::midiCallback, this);

        this->hasError = false;
        this->api = static_cast<QMidi::Api>(midiIn->getCurrentApi());
        this->clientName = clientName;
        this->openedDir = QMidi::UnknownDirection;
        this->error = QMidi::UnspecifiedError;

        sysExOpt = QMidi::KeepUnchanged;
        ignoreOpt = QMidi::AcceptAll;

        sysExBuf.clear();
    }
    catch(RtMidiError& e)
    {
        this->hasError = true;
        this->error = static_cast<QMidi::MidiError>(e.getType());
        this->errorString = QString::fromStdString(e.getMessage());
        qWarning() << "QMidi:" << errorString;
    }
}

QMidiInterface QMidiPrivate::interface(RtMidi& dev, QMidi::Directions dir, unsigned int port)
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

void QMidiPrivate::send(quint8 status)
{
    if(!openedDir.testFlag(QMidi::Output))
    {
        qWarning() << "QMidi: try to send data with input only object. No effect";
        return;
    }

    midiOut->sendMessage(&status, 1);
}

void QMidiPrivate::send(quint8 status, quint8 data1)
{
    static std::vector<unsigned char> v(2, 0);

    if(!openedDir.testFlag(QMidi::Output))
    {
        qWarning() << "QMidi: try to send data with input only object. No effect";
        return;
    }

    if(data1 > dataMax)
    {
        qWarning() <<  "QMidi: data1 is too large. No effect";
        return;
    }

    v[0] = status;
    v[1] = data1;

    midiOut->sendMessage(&v);
}

void QMidiPrivate::send(quint8 status, quint8 data1, quint8 data2)
{
    static std::vector<unsigned char> v(3, 0);

    if(!openedDir.testFlag(QMidi::Output))
    {
        qWarning() << "QMidi: try to send data with input only object. No effect";
        return;
    }

    if(data1 > dataMax)
    {
        qWarning() <<  "QMidi: data1 is too large. No effect";
        return;
    }

    if(data2 > dataMax)
    {
        qWarning() <<  "QMidi: data2 is too large. No effect";
        return;
    }

    v[0] = status;
    v[1] = data1;
    v[2] = data2;

    midiOut->sendMessage(&v);
}

void QMidiPrivate::process(QByteArray data)
{
    Q_ASSERT(!data.isEmpty());
    Q_Q(QMidi);

    q->messageReceived(data);

    quint8 status = data[0];

    switch(status & 0xF0)
    {
    case QMidi::NoteOnStatus:
        q->midiNoteOn(status & 0x0F, data[1], data[2]);
        return;

    case QMidi::NoteOffStatus:
        q->midiNoteOff(status & 0x0F, data[1], data[2]);
        return;

    case QMidi::KeyPressureStatus:
        q->midiKeyPressure(status & 0x0F, data[1], data[2]);
        return;

    case QMidi::ControlChangeStatus:
        q->midiControlChange(status & 0x0F, data[1], data[2]);
        return;

    case QMidi::ProgramChangeStatus:
        q->midiProgramChange(status & 0x0F, data[1]);
        return;

    case QMidi::ChannelPressureStatus:
        q->midiChannelPressure(status & 0x0F, data[1]);
        return;

    case QMidi::PitchBendStatus:
        q->midiPitchBend(status & 0x0F, QMIDI_COMBINE_14BITS(data[2], data[1]));
        return;

    default:
        break;
    }

    switch(status)
    {
    case QMidi::SystemExclusiveMessage:
        if(data.back() == (char)QMidi::EndExclusiveMessage)
            processSysex(data);
        else
            sysExBuf = data;
        break;

    case QMidi::MidiTimeCodeMessage:
        q->midiTimeCode((data[1] >> 4) & 0x0F, data[1] & 0x0F);
        break;

    case QMidi::SongPositionMessage:
        q->midiSongPosition(QMIDI_COMBINE_14BITS(data[2], data[1]));
        break;

    case QMidi::SongSelectMessage:
        q->midiSongSelect(data[1]);
        break;

    case QMidi::TuneRequestMessage:
        q->midiTuneRequest();
        break;

    case QMidi::TimingClockMessage:
        q->midiTimingClock();
        break;

    case QMidi::StartMessage:
        q->midiStart();
        break;

    case QMidi::ContinueMessage:
        q->midiContinue();
        break;

    case QMidi::ActiveSensingMessage:
        q->midiActiveSensing();
        break;

    case QMidi::ResetMessage:
        q->midiReset();
        break;

    default:
    case QMidi::EndExclusiveMessage:
        sysExBuf.append(data);
        if(sysExBuf.back() == (char)QMidi::EndExclusiveMessage)
            processSysex(sysExBuf);
        break;
    }
}

void QMidiPrivate::processSysex(QByteArray data)
{
    Q_ASSERT(data.front() == (char)QMidi::SystemExclusiveMessage);
    Q_ASSERT(data.back() == (char)QMidi::EndExclusiveMessage);

    data.remove(0, 1);
    data.remove(data.size()-1, 1);

    if(data.isEmpty())
        return;

    if(sysExOpt == QMidi::ConvertTo8Bits)
    {
        quint8 s1 = 1;
        quint8 s2 = 6;

        auto c1 = data.begin();
        auto c2 = c1 + 1;

        while(c2 != data.end())
        {
            *c1 <<= s1;
            *c1 += ((*c2 & (1 << s2)) >> s2);

            ++c1;
            ++c2;

            ++s1;
            --s2;

            if(s1 == 8 && c2 != data.end())
            {
                s1 = 1;
                s2 = 6;

                ++c2;
            }
        }

        data.resize(std::distance(data.begin(), c1));
    }

    q_func()->midiSystemExclusive(data);
}

void QMidiPrivate::midiCallback(double timeStamp, std::vector<unsigned char>* msg, void* data)
{
    Q_UNUSED(timeStamp);

    auto that = reinterpret_cast<QMidiPrivate*>(data);

    QByteArray message(reinterpret_cast<const char*>(msg->data()), msg->size());

    that->process(message);
}

void QMidiPrivate::errorCallback(RtMidiError::Type type, const std::string& text, void* data)
{
    auto that = reinterpret_cast<QMidiPrivate*>(data);

    that->error       = static_cast<QMidi::MidiError>(type);
    that->errorString = QString::fromStdString(text);
    that->hasError    = true;

    qWarning() << QMidi::errorToString(that->error) << "-" << that->errorString;
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

    if(i.api() != api())
    {
        qWarning() << "QMidi: Setting an interface with different API. No effect.";
        return;
    }

    if(!i.directions().testFlag(QMidi::Input))
    {
        qWarning() << "QMidi: Setting an output interface on input. No effect.";
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

    if(i.api() != api())
    {
        qWarning() << "QMidi: Setting an interface with different API. No effect.";
        return;
    }

    if(!i.directions().testFlag(QMidi::Output))
    {
        qWarning() << "QMidi: Setting an input interface on output. No effect.";
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

    if(isOpen())
    {
        emit aboutToClose();

        if(d->midiIn->isPortOpen())
            d->midiIn->closePort();

        if(d->midiOut->isPortOpen())
            d->midiOut->closePort();

        d->openedDir = UnknownDirection;
    }
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

void QMidi::sendNoteOff(quint8 chan, quint8 note, quint8 vel)
{
    d_func()->send(QMIDI_COMBINE_STATUS(NoteOffStatus, chan), note, vel);
}

void QMidi::sendNoteOn(quint8 chan, quint8 note, quint8 vel)
{
    d_func()->send(QMIDI_COMBINE_STATUS(NoteOnStatus, chan), note, vel);
}

void QMidi::sendKeyPressure(quint8 chan, quint8 note, quint8 vel)
{
    d_func()->send(QMIDI_COMBINE_STATUS(KeyPressureStatus, chan), note, vel);
}

void QMidi::sendControlChange(quint8 chan, quint8 control, quint8 value)
{
    d_func()->send(QMIDI_COMBINE_STATUS(ControlChangeStatus, chan), control, value);
}

void QMidi::sendProgramChange(quint8 chan, quint8 program)
{
    d_func()->send(QMIDI_COMBINE_STATUS(ProgramChangeStatus, chan), program);
}

void QMidi::sendChannelPressure(quint8 chan, quint8 value)
{
    d_func()->send(QMIDI_COMBINE_STATUS(ChannelPressureStatus, chan), chan, value);
}

void QMidi::sendPitchBend(quint8 chan, qint16 value)
{
    d_func()->send(QMIDI_COMBINE_STATUS(PitchBendStatus, chan),
                   QMIDI_LSB_14BITS(value),
                   QMIDI_MSB_14BITS(value));
}

void QMidi::sendSystemExclusive(QByteArray data)
{
    for(auto& d : data)
        d &= 0x7F;

    data.prepend(SystemExclusiveMessage);
    data.append(EndExclusiveMessage);

    d_func()->send(data);
}

void QMidi::sendTimeCode(quint8 msgType, quint8 value)
{
    d_func()->send(MidiTimeCodeMessage, QMIDI_COMBINE_STATUS((msgType << 4), value) & 0x7F);
}

void QMidi::sendSongPosition(quint16 position)
{
    d_func()->send(SongPositionMessage,
                   QMIDI_LSB_14BITS(position),
                   QMIDI_MSB_14BITS(position));
}

void QMidi::sendSongSelect(quint8 song)
{
    d_func()->send(SongSelectMessage, song & 0x7F);
}

void QMidi::sendTuneRequest()
{
    d_func()->send(TuneRequestMessage);
}

void QMidi::sendTimingClock()
{
    d_func()->send(TimingClockMessage);
}

void QMidi::sendStart()
{
    d_func()->send(StartMessage);
}

void QMidi::sendContinue()
{
    d_func()->send(ContinueMessage);
}

void QMidi::sendStop()
{
    d_func()->send(StopMessage);
}

void QMidi::sendActiveSensing()
{
    d_func()->send(ActiveSensingMessage);
}

void QMidi::sendReset()
{
    d_func()->send(ResetMessage);
}
