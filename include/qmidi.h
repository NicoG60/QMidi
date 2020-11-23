#ifndef QMIDI_H
#define QMIDI_H

#include "qmidi_global.h"
#include <QObject>

class QMidiInterface;
class QMidiPrivate;

#define QMIDI_COMBINE_STATUS(MSB, LSB) ((MSB & 0xF0) + (LSB & 0x0F))
#define QMIDI_COMBINE_14BITS(MSB, LSB) (((MSB & 0x7F) << 7) + (LSB & 0x7F))
#define QMIDI_MSB_14BITS(DATA) ((DATA >> 7) & 0x7F)
#define QMIDI_LSB_14BITS(DATA) (DATA & 0x7F)

#define QMIDI_DATA_MAX (0x7F)

#define QMIDI_DECLARE_NOTE(Suffix) \
    C_##Suffix      = 0, \
    CSharp_##Suffix = 1, \
    DFlat_##Suffix  = 1, \
    D_##Suffix      = 2, \
    DSharp_##Suffix = 3, \
    EFlat_##Suffix  = 3, \
    E_##Suffix      = 4, \
    F_##Suffix      = 5, \
    FSharp_##Suffix = 6, \
    GFlat_##Suffix  = 6, \
    G_##Suffix      = 7, \
    GSharp_##Suffix = 8, \
    AFlat_##Suffix  = 8, \
    A_##Suffix      = 9, \
    ASharp_##Suffix = 10, \
    BFlat_##Suffix  = 10, \
    B_##Suffix      = 11

#define QMIDI_DECLARE_NEXT_NOTE(Suffix, Prec) \
    C_##Suffix      = C_##Prec      + 12, \
    CSharp_##Suffix = CSharp_##Prec + 12, \
    DFlat_##Suffix  = DFlat_##Prec  + 12, \
    D_##Suffix      = D_##Prec      + 12, \
    DSharp_##Suffix = DSharp_##Prec + 12, \
    EFlat_##Suffix  = EFlat_##Prec  + 12, \
    E_##Suffix      = E_##Prec      + 12, \
    F_##Suffix      = F_##Prec      + 12, \
    FSharp_##Suffix = FSharp_##Prec + 12, \
    GFlat_##Suffix  = GFlat_##Prec  + 12, \
    G_##Suffix      = G_##Prec      + 12, \
    GSharp_##Suffix = GSharp_##Prec + 12, \
    AFlat_##Suffix  = AFlat_##Prec  + 12, \
    A_##Suffix      = A_##Prec      + 12, \
    ASharp_##Suffix = ASharp_##Prec + 12, \
    BFlat_##Suffix  = BFlat_##Prec  + 12, \
    B_##Suffix      = B_##Prec      + 12

class QMIDI_EXPORT QMidi : public QObject
{
    Q_OBJECT

public:
    enum Api {
        UnspecifiedApi,
        DefaultApi = UnspecifiedApi,
        CoreMidi,
        ALSA,
        JACK,
        WindowsMM
    };
    Q_ENUM(Api);

    enum Direction {
        UnknownDirection,
        Input,
        Output
    };
    Q_ENUM(Direction);
    Q_DECLARE_FLAGS(Directions, Direction);

    enum MidiError
    {
        Warning,
        DebugWarning,
        UnspecifiedError,
        NoDevicesFound,
        InvalidDevice,
        MemoryError,
        InvalidParameter,
        InvalidUse,
        DriverError,
        SystemError,
        ThreadError
    };
    Q_ENUM(MidiError);

    enum MidiStatus
    {
        NoteOffStatus         = 0x80,
        NoteOnStatus          = 0x90,
        KeyPressureStatus     = 0xA0,
        ControlChangeStatus   = 0xB0,
        ProgramChangeStatus   = 0xC0,
        ChannelPressureStatus = 0xD0,
        PitchBendStatus       = 0xE0,
        SystemStatus          = 0xF0
    };
    Q_ENUM(MidiStatus);

    enum MidiSystemMessage
    {
        SystemExclusiveMessage = SystemStatus + 0x00,
        MidiTimeCodeMessage    = SystemStatus + 0x01,
        SongPositionMessage    = SystemStatus + 0x02,
        SongSelectMessage      = SystemStatus + 0x04,
        // 4 + 5 Undefined
        TuneRequestMessage     = SystemStatus + 0x06,
        EndExclusiveMessage    = SystemStatus + 0x07,
        TimingClockMessage     = SystemStatus + 0x08,
        // 9 Undefined
        StartMessage           = SystemStatus + 0x0A,
        ContinueMessage        = SystemStatus + 0x0B,
        StopMessage            = SystemStatus + 0x0C,
        // D Undefined
        ActiveSensingMessage   = SystemStatus + 0x0E,
        ResetMessage           = SystemStatus + 0x0F
    };
    Q_ENUM(MidiSystemMessage);

    enum MidiNote
    {
        QMIDI_DECLARE_NOTE(m1),
        QMIDI_DECLARE_NEXT_NOTE(0, m1),
        QMIDI_DECLARE_NEXT_NOTE(1, 0),
        QMIDI_DECLARE_NEXT_NOTE(2, 1),
        QMIDI_DECLARE_NEXT_NOTE(3, 2),
        QMIDI_DECLARE_NEXT_NOTE(4, 3),
        QMIDI_DECLARE_NEXT_NOTE(5, 4),
        QMIDI_DECLARE_NEXT_NOTE(6, 5),
        QMIDI_DECLARE_NEXT_NOTE(7, 6),
        QMIDI_DECLARE_NEXT_NOTE(8, 7)
    };
    Q_ENUM(MidiNote);

    enum SysExOptions
    {
        KeepUnchanged,
        ConvertTo8Bits
    };
    Q_ENUM(SysExOptions);

    enum IgnoreOption
    {
        AcceptAll   = 0x00,
        IgnoreSysEx = 0x01,
        IgnoreTime  = 0x02,
        IgnoreSense = 0x04
    };
    Q_ENUM(IgnoreOption);
    Q_DECLARE_FLAGS(IgnoreOptions, IgnoreOption);

public:
    QMidi(QObject* parent = nullptr);
    QMidi(Api api, QObject* parent = nullptr);
    QMidi(Api api, const QString& clientName, QObject* parent = nullptr);
    ~QMidi();

    Api api() const;
    void setApi(Api api);

    QString apiName() const;

    bool hasError() const;
    MidiError error() const;
    QString errorString() const;

    QString clientName() const;
    void setClientName(const QString& name);

    QMidiInterface inputInterface() const;
    void setInputInterface(const QMidiInterface& i);

    QMidiInterface outputInterface() const;
    void setOutputInterface(const QMidiInterface& i);

    void open();
    void openVirtual(const QString& name, Directions dir = UnknownDirection);
    bool isOpen();
    Directions openedDirection();

    void close();

    SysExOptions systemExclusiveOptions() const;
    void setSystemExclusiveOptions(SysExOptions opt);

    IgnoreOptions ignoreOptions() const;
    void setIgnoreOptions(IgnoreOptions opt);

    QList<QMidiInterface> availableInputInterfaces();
    QList<QMidiInterface> availableOutputInterfaces();
    QList<QMidiInterface> availableInterfaces();

    static QList<Api> availableApi();

    static QString apiToString(Api api);
    static QString errorToString(MidiError err);

    static QString version();
    static QString commit();
    static QString rtmidiVersion();
    static QString displayVersion();

signals:
    void apiChanged();
    void clientNameChanged();
    void inputInterfaceChanged();
    void outputInterfaceChanged();
    void aboutToClose();

    void messageReceived(const QByteArray& msg);
    void midiNoteOff(quint8 chan, quint8 note, quint8 vel);
    void midiNoteOn(quint8 chan, quint8 note, quint8 vel);
    void midiKeyPressure(quint8 chan, quint8 note, quint8 vel);
    void midiControlChange(quint8 chan, quint8 control, quint8 value);
    void midiProgramChange(quint8 chan, quint8 program);
    void midiChannelPressure(quint8 chan, quint8 value);
    void midiPitchBend(quint8 chan, qint16 value);

    void midiSystemExclusive(const QByteArray& data);
    void midiTimeCode(quint8 msgType, quint8 value);
    void midiSongPosition(quint16 position);
    void midiSongSelect(quint8 song);
    void midiTuneRequest();

    void midiTimingClock();
    void midiStart();
    void midiContinue();
    void midiStop();
    void midiActiveSensing();
    void midiReset();

    void messageSent();

public slots:
    void sendMessage(const QByteArray& msg);
    void sendNoteOff(quint8 chan, quint8 note, quint8 vel);
    void sendNoteOn(quint8 chan, quint8 note, quint8 vel);
    void sendKeyPressure(quint8 chan, quint8 note, quint8 vel);
    void sendControlChange(quint8 chan, quint8 control, quint8 value);
    void sendProgramChange(quint8 chan, quint8 program);
    void sendChannelPressure(quint8 chan, quint8 value);
    void sendPitchBend(quint8 chan, quint16 value);
    void sendPitchBend(quint8 chan, quint8 msb, quint8 lsb);

    void sendSystemExclusive(QByteArray data);
    void sendTimeCode(quint8 msgType, quint8 value);
    void sendSongPosition(quint16 position);
    void sendSongSelect(quint8 song);
    void sendTuneRequest();

    void sendTimingClock();
    void sendStart();
    void sendContinue();
    void sendStop();
    void sendActiveSensing();
    void sendReset();

private:
    Q_DECLARE_PRIVATE(QMidi);
    Q_DISABLE_COPY(QMidi);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QMidi::Directions);
Q_DECLARE_OPERATORS_FOR_FLAGS(QMidi::IgnoreOptions);

#endif // QMIDI_H
