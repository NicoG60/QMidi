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
    QCOMPARE(_midi.api(), QMidi::CoreMidi);
    QCOMPARE(_midi.clientName(), "QMidi Client");
    QVERIFY(!_midi.hasError());

    QCOMPARE(_midi.inputInterface().index(), -1);
    QCOMPARE(_midi.outputInterface().index(), -1);
}

void midi::test_api_client()
{
    _midi.setApi(QMidi::CoreMidi);
    _midi.setClientName("QMidiTest");

    QCOMPARE(_midi.api(), QMidi::CoreMidi);
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
        QCOMPARE(i.directions(), QMidi::Input);
    }

    for(auto& i : _midi.availableOutputInterfaces())
    {
        QCOMPARE(i.api(), _midi.api());
        QVERIFY(i.index() >= 0);
        QCOMPARE(i.directions(), QMidi::Output);
    }
}

void midi::test_open()
{
    const auto& in  = _midi.availableInputInterfaces().constFirst();
    const auto& out = _midi.availableOutputInterfaces().constFirst();

    _midi.setInputInterface(in);
    _midi.setOutputInterface(out);

    QCOMPARE(_midi.inputInterface(), in);
    QCOMPARE(_midi.outputInterface(), out);

    _midi.open();

    QVERIFY(!_midi.hasError());
    QVERIFY(_midi.isOpen());
    QCOMPARE(_midi.openedDirection(), QMidi::Input | QMidi::Output);

    _midi.close();

    QVERIFY(!_midi.hasError());
    QVERIFY(!_midi.isOpen());
    QCOMPARE(_midi.openedDirection(), QMidi::UnknownDirection);
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
    QCOMPARE(_midi.openedDirection(), QMidi::Input | QMidi::Output);

    _midi.close();

    QVERIFY(!_midi.hasError());
    QVERIFY(!_midi.isOpen());
    QCOMPARE(_midi.openedDirection(), QMidi::UnknownDirection);
#endif
}

QTEST_MAIN(midi)

#include "tst_midi.moc"
