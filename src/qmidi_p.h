#ifndef QMIDI_P_H
#define QMIDI_P_H

#include <qmidi.h>
#include <qmidiinterface.h>
#include <RtMidi.h>
#include <QByteArray>

class QMidiPrivate
{
public:
    Q_DECLARE_PUBLIC(QMidi)

    QMidiPrivate() = default;

    void init(QMidiApi api = UnspecifiedApi, QString clientName = {});

    QMidiInterface interface(RtMidi& dev, QMidiDirections dir, unsigned int port);

    void send(const QByteArray& msg);
    void send(quint8 status);
    void send(quint8 status, quint8 data1);
    void send(quint8 status, quint8 data1, quint8 data2);

    void process(QByteArray data);
    void processSysex(QByteArray data);

    QMidi* q_ptr = nullptr;

    bool hasError;

    QMidiApi api;

    QString clientName;

    QMidiInterface ifaceIn;
    QMidiInterface ifaceOut;

    QMidiDirections openedDir;

    QMidiError error;
    QString errorString;

    QMidiSysExOptions sysExOpt;
    QMidiIgnoreOptions ignoreOpt;

    QScopedPointer<RtMidiIn>  midiIn;
    QScopedPointer<RtMidiOut> midiOut;

    QByteArray sysExBuf;

    static void midiCallback(double timeStamp, std::vector<unsigned char>* msg, void* data);
    static void errorCallback(RtMidiError::Type type, const std::string& text, void* data);

    static constexpr quint8 chanMax = 16;
    static constexpr quint8 dataMax = 0x7F;
};

#endif // QMIDI_P_H
