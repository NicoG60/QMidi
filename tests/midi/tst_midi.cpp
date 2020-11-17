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
    ~midi();

private slots:
    void test_midi();

};

midi::midi()
{
    qDebug() << "Available Midi APIs:";
    for(auto& a : QMidi::availableApi())
        qDebug() << "-" << QMidi::apiToString(a);
}

midi::~midi()
{

}

void midi::test_midi()
{
    QMidi midi(QMidi::CoreMidi, "QMidiTest");

    QCOMPARE(midi.api(), QMidi::CoreMidi);
    QCOMPARE(midi.clientName(), "QMidiTest");

    for(auto& i : midi.availableInterfaces())
        qDebug() << i;
}

QTEST_MAIN(midi)

#include "tst_midi.moc"
