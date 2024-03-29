#include <qmidi.h>

#include "qmidi_p.h"
#include "qmidiinterface_p.h"

#include <QDebug>

#define _STR(x) #x
#define STRINGIFY(x) _STR(x)

void QMidiPrivate::init(QMidiApi api, QString clientName)
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
        this->api = static_cast<QMidiApi>(midiIn->getCurrentApi());
        this->clientName = clientName;
        this->openedDir = UnknownDirection;
        this->error = UnspecifiedError;

        sysExOpt = KeepUnchanged;
        ignoreOpt = IgnoreSysEx | IgnoreTime | IgnoreSense;

        sysExBuf.clear();
    }
    catch(RtMidiError& e)
    {
        this->hasError = true;
        this->error = static_cast<QMidiError>(e.getType());
        this->errorString = QString::fromStdString(e.getMessage());
        qWarning() << "QMidi:" << errorString;
    }
}

QMidiInterface QMidiPrivate::interface(RtMidi& dev, QMidiDirections dir, unsigned int port)
{
    if(hasError)
    {
        qWarning() << "QMidi: Query for interface while error occured";
        return {};
    }

    QMidiInterface i;
    i.d_ptr->api       = api;
    i.d_ptr->direction = dir;
    i.d_ptr->name      = QString::fromStdString(dev.getPortName(port));
    i.d_ptr->index     = port;

    return i;
}

void QMidiPrivate::send(const QByteArray& msg)
{
    if(!openedDir.testFlag(Output))
    {
        qWarning() << "QMidi: try to send data with input only object. No effect";
        return;
    }

    midiOut->sendMessage(reinterpret_cast<const unsigned char*>(msg.data()), msg.size());
    emit q_func()->messageSent();
}

void QMidiPrivate::send(quint8 status)
{
    if(!openedDir.testFlag(Output))
    {
        qWarning() << "QMidi: try to send data with input only object. No effect";
        return;
    }

    midiOut->sendMessage(&status, 1);
    emit q_func()->messageSent();
}

void QMidiPrivate::send(quint8 status, quint8 data1)
{
    static std::vector<unsigned char> v(2, 0);

    if(!openedDir.testFlag(Output))
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
    emit q_func()->messageSent();
}

void QMidiPrivate::send(quint8 status, quint8 data1, quint8 data2)
{
    static std::vector<unsigned char> v(3, 0);

    if(!openedDir.testFlag(Output))
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
    emit q_func()->messageSent();
}

void QMidiPrivate::process(QByteArray data)
{
    Q_ASSERT(!data.isEmpty());
    Q_Q(QMidi);

    emit q->messageReceived(data);

    quint8 status = data[0];

    switch(status & 0xF0)
    {
    case NoteOnStatus:
        emit q->midiNoteOn(status & 0x0F, data[1], data[2]);
        return;

    case NoteOffStatus:
        emit q->midiNoteOff(status & 0x0F, data[1], data[2]);
        return;

    case KeyPressureStatus:
        emit q->midiKeyPressure(status & 0x0F, data[1], data[2]);
        return;

    case ControlChangeStatus:
        emit q->midiControlChange(status & 0x0F, data[1], data[2]);
        return;

    case ProgramChangeStatus:
        emit q->midiProgramChange(status & 0x0F, data[1]);
        return;

    case ChannelPressureStatus:
        emit q->midiChannelPressure(status & 0x0F, data[1]);
        return;

    case PitchBendStatus:
        emit q->midiPitchBend(status & 0x0F, QMIDI_COMBINE_14BITS(data[2], data[1]));
        return;

    default:
        break;
    }

    switch(status)
    {
    case SystemExclusiveMessage:
        if(data.back() == (char)EndExclusiveMessage)
            processSysex(data);
        else
            sysExBuf = data;
        break;

    case MidiTimeCodeMessage:
        emit q->midiTimeCode((data[1] >> 4) & 0x0F, data[1] & 0x0F);
        break;

    case SongPositionMessage:
        emit q->midiSongPosition(QMIDI_COMBINE_14BITS(data[2], data[1]));
        break;

    case SongSelectMessage:
        emit q->midiSongSelect(data[1]);
        break;

    case TuneRequestMessage:
        emit q->midiTuneRequest();
        break;

    case TimingClockMessage:
        emit q->midiTimingClock();
        break;

    case StartMessage:
        emit q->midiStart();
        break;

    case ContinueMessage:
        emit q->midiContinue();
        break;

    case ActiveSensingMessage:
        emit q->midiActiveSensing();
        break;

    case ResetMessage:
        emit q->midiReset();
        break;

    default:
    case EndExclusiveMessage:
        sysExBuf.append(data);
        if(sysExBuf.back() == (char)EndExclusiveMessage)
            processSysex(sysExBuf);
        break;
    }
}

void QMidiPrivate::processSysex(QByteArray data)
{
    Q_ASSERT(data.front() == (char)SystemExclusiveMessage);
    Q_ASSERT(data.back() == (char)EndExclusiveMessage);

    data.remove(0, 1);
    data.remove(data.size()-1, 1);

    if(data.isEmpty())
        return;

    if(sysExOpt == ConvertTo8Bits)
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

    emit q_func()->midiSystemExclusive(data);
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

    that->error       = static_cast<QMidiError>(type);
    that->errorString = QString::fromStdString(text);
    that->hasError    = true;

    qWarning() << QMidi::errorToString(that->error) << "-" << that->errorString;
}



// =============================================================================




QMidi::QMidi(QObject* parent) :
    QObject(parent),
    d_ptr(new QMidiPrivate)
{
    Q_D(QMidi);
    d->q_ptr = this;
    d->init();
}

QMidi::QMidi(QMidiApi api, QObject* parent) :
    QObject(parent),
    d_ptr(new QMidiPrivate)
{
    Q_D(QMidi);
    d->q_ptr = this;
    d->init(api);
}

QMidi::QMidi(QMidiApi api, const QString& clientName, QObject* parent) :
    QObject(parent),
    d_ptr(new QMidiPrivate)
{
    Q_D(QMidi);
    d->q_ptr = this;
    d->init(api, clientName);
}

QMidi::~QMidi()
{
    if(isOpen())
        close();
}

QMidiApi QMidi::api() const
{
    return d_func()->api;
}

void QMidi::setApi(QMidiApi api)
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

QMidiError QMidi::error() const
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

    if(!i.directions().testFlag(Input))
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

    if(!i.directions().testFlag(Output))
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

        d->midiIn->openPort(idx, name);

        if(!hasError())
            d->openedDir |= Input;
    }

    if(d->ifaceOut.isValid())
    {
        auto idx = d->ifaceOut.index();
        auto name = d->ifaceOut.name().toStdString();

        d->midiOut->openPort(idx, name);

        if(!hasError())
            d->openedDir |= Output;
    }

    if(d->openedDir == UnknownDirection)
        qWarning() << "QMidi: Error occured while trying to open midi devices.";
    else
        emit isOpenChanged();
}

void QMidi::openVirtual(const QString& name, QMidiDirections dir)
{
    Q_D(QMidi);

#ifdef Q_OS_WIN
    d->error = QMidiError::InvalidUse;
    d->errorString = "QMidi: Virtual Ports are not supported on Windows.";
    qWarning() << d->errorString;
    return;
#endif

    if(dir == UnknownDirection)
        dir = Input | Output;

    if(dir.testFlag(Input))
    {
        d->midiIn->openVirtualPort(name.toStdString());

        if(!hasError())
        {
            for(const auto& i : availableOutputInterfaces())
            {
                if(i.name() == name)
                {
                    if(d->ifaceIn != i)
                    {
                        d->ifaceIn = i;
                        emit inputInterfaceChanged();
                    }
                    break;
                }
            }


            d->openedDir |= Input;
        }
    }

    if(dir.testFlag(Output))
    {
        d->midiOut->openVirtualPort(name.toStdString());

        if(!hasError())
        {
            for(const auto& i : availableInputInterfaces())
            {
                if(i.name() == name)
                {
                    if(d->ifaceOut != i)
                    {
                        d->ifaceOut = i;
                        emit outputInterfaceChanged();
                    }
                    break;
                }
            }


            d->openedDir |= Output;
        }
    }

    if(d->openedDir == UnknownDirection)
        qWarning() << "QMidi: Error occured while trying to open midi devices.";
    else
        emit isOpenChanged();
}

bool QMidi::isOpen()
{
    return openedDirection() != UnknownDirection;
}

QMidiDirections QMidi::openedDirection()
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
        emit isOpenChanged();
    }
}

QMidiSysExOptions QMidi::systemExclusiveOptions() const
{
    return d_func()->sysExOpt;
}

void QMidi::setSystemExclusiveOptions(QMidiSysExOptions opt)
{
    d_func()->sysExOpt = opt;
}

QMidiIgnoreOptions QMidi::ignoreOptions() const
{
    return d_func()->ignoreOpt;
}

void QMidi::setIgnoreOptions(QMidiIgnoreOptions opt)
{
    Q_D(QMidi);

    if(d->ignoreOpt != opt)
    {
        d->ignoreOpt = opt;

        d->midiIn->ignoreTypes(opt.testFlag(IgnoreSysEx),
                               opt.testFlag(IgnoreTime),
                               opt.testFlag(IgnoreSense));
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

QList<QMidiApi> QMidi::availableApi()
{
    QList<QMidiApi> r;
    std::vector<RtMidi::Api> v;

    RtMidi::getCompiledApi(v);

    for(auto a : v)
        r << static_cast<QMidiApi>(a);

    return r;
}

QString QMidi::apiToString(QMidiApi api)
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

QString QMidi::errorToString(QMidiError err)
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

QString QMidi::version()
{
    return QStringLiteral(STRINGIFY(SOFT_VERSION));
}

QString QMidi::commit()
{
    return QStringLiteral(STRINGIFY(GIT_VERSION));
}

QString QMidi::rtmidiVersion()
{
    return QString::fromStdString(RtMidi::getVersion());
}

QString QMidi::displayVersion()
{
    return QStringLiteral("QMidi v%1 (%2) - RtMidi v%3").arg(version(), commit(), rtmidiVersion());
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

void QMidi::sendPitchBend(quint8 chan, quint16 value)
{
    d_func()->send(QMIDI_COMBINE_STATUS(PitchBendStatus, chan),
                   QMIDI_LSB_14BITS(value),
                   QMIDI_MSB_14BITS(value));
}

void QMidi::sendPitchBend(quint8 chan, quint8 msb, quint8 lsb)
{
    d_func()->send(QMIDI_COMBINE_STATUS(PitchBendStatus, chan),
                   lsb,
                   msb);
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
