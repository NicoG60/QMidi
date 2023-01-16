#include <QtTest>
#include <QCoreApplication>

#include <QMidi>
#include <QMidiInterface>
#include <QDebug>

class midi : public QObject
{
    Q_OBJECT

public:
    midi();
    ~midi() override = default;

private slots:
    void test_ctor();

    void test_api_client();

    void test_midi_interfaces();

    void test_open();

    void test_virtual();

private:
    QMidi _midi;
};

midi::midi()
{
    qDebug() << "Available Midi APIs:";
    for(auto& a : QMidi::availableApi())
        qDebug() << "-" << QMidi::apiToString(a);
}

void midi::test_ctor()
{
#if defined(Q_OS_MAC)
    QCOMPARE(_midi.api(), QMidiApi::CoreMidi);
#elif defined(Q_OS_WIN)
    QCOMPARE(_midi.api(), QMidiApi::WindowsMM);
#else
    if(_midi.api() != QMidiApi::ALSA && _midi.api() != QMidiApi::JACK)
        QFAIL("Bad API for Linux");
#endif
    QCOMPARE(_midi.clientName(), "QMidi Client");
    QVERIFY(!_midi.hasError());

    QCOMPARE(_midi.inputInterface().index(), -1);
    QCOMPARE(_midi.outputInterface().index(), -1);
}

void midi::test_api_client()
{
#ifdef Q_OS_MAC
    _midi.setApi(QMidiApi::CoreMidi);
#elif Q_OS_WIN
    _midi.setApi(QMidiApi::WindowsMM);
#else
    _midi.setApi(QMidiApi::ALSA);
#endif
    _midi.setClientName("QMidiTest");

#ifdef Q_OS_MAC
    QCOMPARE(_midi.api(), QMidiApi::CoreMidi);
#elif Q_OS_WIN
    QCOMPARE(_midi.api(), QMidiApi::WindowsMM);
#else
    QCOMPARE(_midi.api(), QMidiApi::ALSA);
#endif
    QCOMPARE(_midi.clientName(), "QMidiTest");
    QVERIFY(!_midi.hasError());
}

void midi::test_midi_interfaces()
{
    QVERIFY(!_midi.hasError());

    for(auto& i : _midi.availableInputInterfaces())
    {
        QCOMPARE(i.api(), _midi.api());
        QVERIFY(i.index() >= 0);
        QCOMPARE(i.directions(), QMidiDirection::Input);
    }

    for(auto& i : _midi.availableOutputInterfaces())
    {
        QCOMPARE(i.api(), _midi.api());
        QVERIFY(i.index() >= 0);
        QCOMPARE(i.directions(), QMidiDirection::Output);
    }
}

void midi::test_open()
{
    QMidiDirections dir = QMidiDirection::UnknownDirection;
    if(!_midi.availableInputInterfaces().isEmpty())
    {
        const auto& in  = _midi.availableInputInterfaces().constFirst();
        _midi.setInputInterface(in);
        QCOMPARE(_midi.inputInterface(), in);
        dir |= QMidiDirection::Input;
    }

    if(!_midi.availableOutputInterfaces().isEmpty())
    {
        const auto& out = _midi.availableOutputInterfaces().constFirst();
        _midi.setOutputInterface(out);
        QCOMPARE(_midi.outputInterface(), out);
        dir |= QMidiDirection::Output;
    }

    if(dir == QMidiDirection::UnknownDirection)
        QSKIP("No available MIDI device to test");
    else
    {
        _midi.open();

        QVERIFY(!_midi.hasError());
        QVERIFY(_midi.isOpen());
        QCOMPARE(_midi.openedDirection(), dir);

        _midi.close();

        QVERIFY(!_midi.hasError());
        QVERIFY(!_midi.isOpen());
        QCOMPARE(_midi.openedDirection(), QMidiDirection::UnknownDirection);
    }
}

void midi::test_virtual()
{
#ifdef Q_OS_WIN
    QSKIP("Virtual ports dont work yet on Windows");
#else
    _midi.openVirtual("Virtual");

    QCOMPARE(_midi.inputInterface().name(), "Virtual");
    QCOMPARE(_midi.outputInterface().name(), "Virtual");
    QVERIFY(!_midi.hasError());
    QVERIFY(_midi.isOpen());
    QCOMPARE(_midi.openedDirection(), QMidiDirection::Input | QMidiDirection::Output);

    _midi.close();

    QVERIFY(!_midi.hasError());
    QVERIFY(!_midi.isOpen());
    QCOMPARE(_midi.openedDirection(), QMidiDirection::UnknownDirection);
#endif
}

QTEST_MAIN(midi)

#include "tst_midi.moc"
