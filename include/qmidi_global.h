#ifndef QMIDI_GLOBAL_H
#define QMIDI_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QMIDI_LIBRARY)
#  define QMIDI_EXPORT Q_DECL_EXPORT
#else
#  define QMIDI_EXPORT Q_DECL_IMPORT
#endif

#define MIDI_STATUS_NOTEOFF         0x80
#define MIDI_STATUS_NOTEON          0x90
#define MIDI_STATUS_KEYPRESURE      0xa0
#define MIDI_STATUS_CONTROLCHANGE   0xb0
#define MIDI_STATUS_PROGRAMCHANGE   0xc0
#define MIDI_STATUS_CHANNELPRESSURE 0xd0
#define MIDI_STATUS_PITCHBEND       0xe0
#define MIDI_STATUS_SYSEX           0xf0
#define MIDI_STATUS_ENDSYSEX        0xf7
#define MIDI_STATUS_REALTIME        0xf8

#define MIDI_STATUS_MASK            0xf0
#define MIDI_CHANNEL_MASK           0x0f

#define MIDI_LSB(x) static_cast<quint8>(x % 0x80)
#define MIDI_MSB(x) static_cast<quint8>(x / 0x80)

#define Cm1  (0)
#define Csm1 (1)
#define Dm1  (2)
#define Dsm1 (3)
#define Em1  (4)
#define Fm1  (5)
#define Fsm1 (6)
#define Gm1  (7)
#define Gsm1 (8)
#define Am1  (9)
#define Asm1 (10)
#define Bm1  (11)

#define C_0  (Cm1  + 12)
#define Cs_0 (Csm1 + 12)
#define D_0  (Dm1  + 12)
#define Ds_0 (Dsm1 + 12)
#define E_0  (Em1  + 12)
#define F_0  (Fm1  + 12)
#define Fs_0 (Fsm1 + 12)
#define G_0  (Gm1  + 12)
#define Gs_0 (Gsm1 + 12)
#define A_0  (Am1  + 12)
#define As_0 (Asm1 + 12)
#define B_0  (Bm1  + 12)

#define  C_1  ( C_0  + 12)
#define Cs_1  (Cs_0  + 12)
#define  D_1  ( D_0  + 12)
#define Ds_1  (Ds_0  + 12)
#define  E_1  ( E_0  + 12)
#define  F_1  ( F_0  + 12)
#define Fs_1  (Fs_0  + 12)
#define  G_1  ( G_0  + 12)
#define Gs_1  (Gs_0  + 12)
#define  A_1  ( A_0  + 12)
#define As_1  (As_0  + 12)
#define  B_1  ( B_0  + 12)

#define  C_2  ( C_1  + 12)
#define Cs_2  (Cs_1  + 12)
#define  D_2  ( D_1  + 12)
#define Ds_2  (Ds_1  + 12)
#define  E_2  ( E_1  + 12)
#define  F_2  ( F_1  + 12)
#define Fs_2  (Fs_1  + 12)
#define  G_2  ( G_1  + 12)
#define Gs_2  (Gs_1  + 12)
#define  A_2  ( A_1  + 12)
#define As_2  (As_1  + 12)
#define  B_2  ( B_1  + 12)

#define  C_3  ( C_2  + 12)
#define Cs_3  (Cs_2  + 12)
#define  D_3  ( D_2  + 12)
#define Ds_3  (Ds_2  + 12)
#define  E_3  ( E_2  + 12)
#define  F_3  ( F_2  + 12)
#define Fs_3  (Fs_2  + 12)
#define  G_3  ( G_2  + 12)
#define Gs_3  (Gs_2  + 12)
#define  A_3  ( A_2  + 12)
#define As_3  (As_2  + 12)
#define  B_3  ( B_2  + 12)

#define  C_4  ( C_3  + 12)
#define Cs_4  (Cs_3  + 12)
#define  D_4  ( D_3  + 12)
#define Ds_4  (Ds_3  + 12)
#define  E_4  ( E_3  + 12)
#define  F_4  ( F_3  + 12)
#define Fs_4  (Fs_3  + 12)
#define  G_4  ( G_3  + 12)
#define Gs_4  (Gs_3  + 12)
#define  A_4  ( A_3  + 12)
#define As_4  (As_3  + 12)
#define  B_4  ( B_3  + 12)

#define  C_5  ( C_4  + 12)
#define Cs_5  (Cs_4  + 12)
#define  D_5  ( D_4  + 12)
#define Ds_5  (Ds_4  + 12)
#define  E_5  ( E_4  + 12)
#define  F_5  ( F_4  + 12)
#define Fs_5  (Fs_4  + 12)
#define  G_5  ( G_4  + 12)
#define Gs_5  (Gs_4  + 12)
#define  A_5  ( A_4  + 12)
#define As_5  (As_4  + 12)
#define  B_5  ( B_4  + 12)

#define  C_6  ( C_5  + 12)
#define Cs_6  (Cs_5  + 12)
#define  D_6  ( D_5  + 12)
#define Ds_6  (Ds_5  + 12)
#define  E_6  ( E_5  + 12)
#define  F_6  ( F_5  + 12)
#define Fs_6  (Fs_5  + 12)
#define  G_6  ( G_5  + 12)
#define Gs_6  (Gs_5  + 12)
#define  A_6  ( A_5  + 12)
#define As_6  (As_5  + 12)
#define  B_6  ( B_5  + 12)

#define  C_7  ( C_6  + 12)
#define Cs_7  (Cs_6  + 12)
#define  D_7  ( D_6  + 12)
#define Ds_7  (Ds_6  + 12)
#define  E_7  ( E_6  + 12)
#define  F_7  ( F_6  + 12)
#define Fs_7  (Fs_6  + 12)
#define  G_7  ( G_6  + 12)
#define Gs_7  (Gs_6  + 12)
#define  A_7  ( A_6  + 12)
#define As_7  (As_6  + 12)
#define  B_7  ( B_6  + 12)

#define  C_8  ( C_7  + 12)
#define Cs_8  (Cs_7  + 12)
#define  D_8  ( D_7  + 12)
#define Ds_8  (Ds_7  + 12)
#define  E_8  ( E_7  + 12)
#define  F_8  ( F_7  + 12)
#define Fs_8  (Fs_7  + 12)
#define  G_8  ( G_7  + 12)
#define Gs_8  (Gs_7  + 12)
#define  A_8  ( A_7  + 12)
#define As_8  (As_7  + 12)
#define  B_8  ( B_7  + 12)

#endif // QMIDI_GLOBAL_H
