#ifndef QMIDI_P_H
#define QMIDI_P_H

#include <private/qobject_p.h>
#include <qmidi.h>
#include <qmidiinterface.h>
#include <RtMidi.h>

class QMidiPrivate : public QObjectPrivate
{
public:
    QMidiPrivate();

    void init(QMidi::Api api = QMidi::UnspecifiedApi, const QString clientName = {});

    QMidiInterface interface(RtMidi& dev, unsigned int port);
    QMidiInterface makeVirtual(const QString& name);

    bool hasError;

    QMidi::Api api;

    QString clientName;

    QMidiInterface ifaceIn;
    QMidiInterface ifaceOut;

    QMidi::Directions openedDir;

    QMidi::MidiError error;
    QString errorString;

    QScopedPointer<RtMidiIn>  midiIn;
    QScopedPointer<RtMidiOut> midiOut;
};

#endif // QMIDI_P_H
