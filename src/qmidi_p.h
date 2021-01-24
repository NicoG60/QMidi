#ifndef QMIDI_P_H
#define QMIDI_P_H

#include <private/qobject_p.h>
#include <qmidi.h>
#include <qmidiinterface.h>
#include <RtMidi.h>
#include <QByteArray>

class QMidiPrivate : public QObjectPrivate
{
public:
    Q_DECLARE_PUBLIC(QMidi)

    QMidiPrivate() = default;

    void init(QMidi::Api api = QMidi::UnspecifiedApi, QString clientName = {});

    QMidiInterface interface(RtMidi& dev, QMidi::Directions dir, unsigned int port);

    void send(const QByteArray& msg);
    void send(quint8 status);
    void send(quint8 status, quint8 data1);
    void send(quint8 status, quint8 data1, quint8 data2);

    void process(QByteArray data);
    void processSysex(QByteArray data);

    bool hasError;

    QMidi::Api api;

    QString clientName;

    QMidiInterface ifaceIn;
    QMidiInterface ifaceOut;

    QMidi::Directions openedDir;

    QMidi::MidiError error;
    QString errorString;

    QMidi::SysExOptions sysExOpt;
    QMidi::IgnoreOptions ignoreOpt;

    QScopedPointer<RtMidiIn>  midiIn;
    QScopedPointer<RtMidiOut> midiOut;

    QByteArray sysExBuf;

    static void midiCallback(double timeStamp, std::vector<unsigned char>* msg, void* data);
    static void errorCallback(RtMidiError::Type type, const std::string& text, void* data);

    static constexpr quint8 chanMax = 16;
    static constexpr quint8 dataMax = 0x7F;
};

#endif // QMIDI_P_H
