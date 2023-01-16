#ifndef QMIDI_H
#define QMIDI_H

#include "qmidi_global.h"
#include "qmidiinterface.h"
#include <QObject>

class QMidiInterface;
class QMidiPrivate;

#define QMIDI_COMBINE_STATUS(MSB, LSB) ((MSB & 0xF0) + (LSB & 0x0F))
#define QMIDI_COMBINE_14BITS(MSB, LSB) (((MSB & 0x7F) << 7) + (LSB & 0x7F))
#define QMIDI_MSB_14BITS(DATA) ((DATA >> 7) & 0x7F)
#define QMIDI_LSB_14BITS(DATA) (DATA & 0x7F)

#define QMIDI_DATA_MAX (0x7F)

class QMIDI_EXPORT QMidi : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QMidiApi api READ api WRITE setApi NOTIFY apiChanged)
    Q_PROPERTY(QString apiName READ apiName NOTIFY apiChanged)
    Q_PROPERTY(QString clientName READ clientName WRITE setClientName NOTIFY clientNameChanged)
    Q_PROPERTY(QMidiInterface inputInterface READ inputInterface WRITE setInputInterface NOTIFY inputInterfaceChanged)
    Q_PROPERTY(QMidiInterface outputInterface READ outputInterface WRITE setOutputInterface NOTIFY outputInterfaceChanged)
    Q_PROPERTY(QMidiSysExOptions systemExclusiveOptions READ systemExclusiveOptions WRITE setSystemExclusiveOptions)
    Q_PROPERTY(QMidiIgnoreOptions ignoreOptions READ ignoreOptions WRITE setIgnoreOptions)
    Q_PROPERTY(bool isOpen READ isOpen NOTIFY isOpenChanged)

    Q_PROPERTY(QList<QMidiInterface> availableInputInterfaces READ availableInputInterfaces)
    Q_PROPERTY(QList<QMidiInterface> availableOutputInterfaces READ availableOutputInterfaces)

public:
    QMidi(QObject* parent = nullptr);
    QMidi(QMidiApi api, QObject* parent = nullptr);
    QMidi(QMidiApi api, const QString& clientName, QObject* parent = nullptr);
    ~QMidi() override;

    QMidiApi api() const;
    void setApi(QMidiApi api);

    QString apiName() const;

    bool hasError() const;
    QMidiError error() const;
    QString errorString() const;

    QString clientName() const;
    void setClientName(const QString& name);

    QMidiInterface inputInterface() const;
    void setInputInterface(const QMidiInterface& i);

    QMidiInterface outputInterface() const;
    void setOutputInterface(const QMidiInterface& i);

    void open();
    void openVirtual(const QString& name, QMidiDirections dir = UnknownDirection);
    bool isOpen();
    QMidiDirections openedDirection();

    void close();

    QMidiSysExOptions systemExclusiveOptions() const;
    void setSystemExclusiveOptions(QMidiSysExOptions opt);

    QMidiIgnoreOptions ignoreOptions() const;
    void setIgnoreOptions(QMidiIgnoreOptions opt);

    QList<QMidiInterface> availableInputInterfaces();
    QList<QMidiInterface> availableOutputInterfaces();
    QList<QMidiInterface> availableInterfaces();

    Q_INVOKABLE static QList<QMidiApi> availableApi();

    static QString apiToString(QMidiApi api);
    static QString errorToString(QMidiError err);

    static QString version();
    static QString commit();
    static QString rtmidiVersion();
    static QString displayVersion();

signals:
    void apiChanged();
    void clientNameChanged();
    void inputInterfaceChanged();
    void outputInterfaceChanged();
    void isOpenChanged();
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

    QScopedPointer<QMidiPrivate> d_ptr;
};

#endif // QMIDI_H
