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

    void init(QMidi::Api api = QMidi::UnspecifiedApi, QString clientName = {});

    QMidiInterface interface(RtMidi& dev, QMidi::Direction dir, unsigned int port);
    QMidiInterface makeVirtual(QString name = {});

    void send(const QByteArray& msg);
    void send(quint8 status, quint8 chan, quint8 data1);
    void send(quint8 status, quint8 chan, quint8 data1, quint8 data2);

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
