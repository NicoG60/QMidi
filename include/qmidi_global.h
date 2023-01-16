#ifndef QMIDI_GLOBAL_H
#define QMIDI_GLOBAL_H

#include <QtCore/qmetatype.h>
#include <QtCore/qglobal.h>

#if defined(QMIDI_LIBRARY)
#  define QMIDI_EXPORT Q_DECL_EXPORT
#else
#  define QMIDI_EXPORT Q_DECL_IMPORT
#endif

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

enum QMidiApi {
    UnspecifiedApi,
    DefaultApi = UnspecifiedApi,
    CoreMidi,
    ALSA,
    JACK,
    WindowsMM
};
Q_DECLARE_METATYPE(QMidiApi);

enum QMidiDirection {
    UnknownDirection,
    Input,
    Output
};
Q_DECLARE_METATYPE(QMidiDirection);
Q_DECLARE_FLAGS(QMidiDirections, QMidiDirection);

enum QMidiError
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
Q_DECLARE_METATYPE(QMidiError);

enum QMidiStatus
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
Q_DECLARE_METATYPE(QMidiStatus);

enum QMidiSystemMessage
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
Q_DECLARE_METATYPE(QMidiSystemMessage);

enum QMidiNote
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
Q_DECLARE_METATYPE(QMidiNote);

enum QMidiSysExOptions
{
    KeepUnchanged,
    ConvertTo8Bits
};
Q_DECLARE_METATYPE(QMidiSysExOptions);

enum QMidiIgnoreOption
{
    AcceptAll   = 0x00,
    IgnoreSysEx = 0x01,
    IgnoreTime  = 0x02,
    IgnoreSense = 0x04
};
Q_DECLARE_METATYPE(QMidiIgnoreOption);
Q_DECLARE_FLAGS(QMidiIgnoreOptions, QMidiIgnoreOption);

Q_DECLARE_OPERATORS_FOR_FLAGS(QMidiDirections);
Q_DECLARE_OPERATORS_FOR_FLAGS(QMidiIgnoreOptions);

#endif // QMIDI_GLOBAL_H


