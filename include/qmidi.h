#ifndef QMIDI_H
#define QMIDI_H

#include "qmidi_global.h"
#include <QObject>


class QMidiInterface;
class QMidiPrivate;

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
        WindowsMultimedia
    };
    Q_ENUM(Api);

    enum Direction {
        UnknownDirection,
        Input,
        Output,
        InputOutput
    };
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

public:
    QMidi(QObject* parent = nullptr);
    ~QMidi();

    Api api() const;
    void setApi(Api api);

    QString apiName() const;

    MidiError error() const;
    QString errorString() const;

    QString clientName() const;
    void setClientName(const QString& name);

    QMidiInterface inputInterface() const;
    void setInputInterface(const QMidiInterface& i);

    QMidiInterface outputInterface() const;
    void setOutputInterface(const QMidiInterface& i);

    void open();
    bool isOpen();
    Directions openedDirection();

    static QList<Api> availableApi();

    static QList<QMidiInterface> availableInputInterfaces();
    static QList<QMidiInterface> availableOutputInterfaces();
    static QList<QMidiInterface> availableInterfaces();

    static QMidiInterface createVirtualInterface(const QString& name);

    static QString apiToString(Api api);
    static QString errorToString(MidiError err);

signals:
    void apiChanged();
    void clientNameChanged();
    void inputInterfaceChanged();
    void outputInterfaceChanged();
    void aboutToClose();

    void messageReceived(const QByteArray&);
    void midiNoteOff		(const quint8 chan, const quint8 note,	  const quint8 vel);
    void midiNoteOn			(const quint8 chan, const quint8 note,	  const quint8 vel);
    void midiKeyPressure	(const quint8 chan, const quint8 note,	  const quint8 vel);
    void midiControler		(const quint8 chan, const quint8 control, const quint8 value);
    void midiProgram		(const quint8 chan, const quint8 program);
    void midiChannelPressure(const quint8 chan, const quint8 value);
    void midiPitchBend		(const quint8 chan, const qint16 value);
    void midiSysex			(const QByteArray &data);
    void midiSystemCommon	(const quint8 status);
    void midiSystemRealtime	(const quint8 status);

public slots:
    void sendNoteOff		(const quint8 chan, const quint8 note,	  const quint8 vel);
    void sendNoteOn			(const quint8 chan, const quint8 note,	  const quint8 vel);
    void sendKeyPressure	(const quint8 chan, const quint8 note,	  const quint8 vel);
    void sendControler		(const quint8 chan, const quint8 control, const quint8 value);
    void sendProgram		(const quint8 chan, const quint8 program);
    void sendChannelPressure(const quint8 chan, const quint8 value);
    void sendPitchBend		(const quint8 chan, const qint16 value);
    void sendSysex			(const QByteArray &data);

private:
    Q_DECLARE_PRIVATE(QMidi);
    Q_DISABLE_COPY(QMidi);
};

Q_DECLARE_OPERATORS_FOR_FLAGS(QMidi::Directions);

#endif // QMIDI_H
