#include <QtTest/QtTest>
#include "loglistener.h"
#include "gameevents/statushostname.h"
#include "gameevents/statusplayernumbers.h"
#include "gameevents/statusmap.h"
#include "gameevents/statusipaddress.h"

class TestStatusCommand : public QObject {
    Q_OBJECT

    morgoth::LogListener* logListener = nullptr;

private slots:
    void init();
    void cleanup();

    void hostname();
    void players();
    void map();
    void ipAddress();
};



void TestStatusCommand::init()
{
    Q_ASSERT(logListener == nullptr);

    QString fileName = QFINDTESTDATA("cmd_status.log");
    QVERIFY(QFile(fileName).exists());

    logListener = new morgoth::LogListener(fileName);
    QVERIFY(logListener);
}

void TestStatusCommand::cleanup()
{
    Q_ASSERT(logListener);
    logListener->requestInterruption();
    logListener->wait();
    delete logListener;
    logListener = nullptr;
}

void TestStatusCommand::hostname()
{
    Q_ASSERT(logListener);

    morgoth::StatusHostname* hostnameLine = new morgoth::StatusHostname;
    QSignalSpy spy(hostnameLine, &morgoth::EventHandler::activated);
    logListener->installEventHandler(hostnameLine);

    logListener->start();
    spy.wait();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(hostnameLine->hostname(), "melkor.tf #1");
}

void TestStatusCommand::players()
{
    Q_ASSERT(logListener);

    morgoth::StatusPlayerNumbers* playerLine = new morgoth::StatusPlayerNumbers;
    QSignalSpy spy(playerLine, &morgoth::EventHandler::activated);
    logListener->installEventHandler(playerLine);

    logListener->start();
    spy.wait();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(playerLine->playerCount(), 0);
    QCOMPARE(playerLine->maxPlayers(), 25);
}

void TestStatusCommand::map()
{
    Q_ASSERT(logListener);

    morgoth::StatusMap* mapLine = new morgoth::StatusMap;
    QSignalSpy spy(mapLine, &morgoth::EventHandler::activated);
    logListener->installEventHandler(mapLine);

    logListener->start();
    spy.wait();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(mapLine->map(), "cp_process_final");
}

void TestStatusCommand::ipAddress()
{
    Q_ASSERT(logListener);

    morgoth::StatusIpAddress* ipAddressLine = new morgoth::StatusIpAddress;
    QSignalSpy spy(ipAddressLine, &morgoth::EventHandler::activated);
    logListener->installEventHandler(ipAddressLine);

    logListener->start();
    spy.wait();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(ipAddressLine->ip(), "151.80.108.144");
    QCOMPARE(ipAddressLine->port(), 27015);
}

QTEST_MAIN(TestStatusCommand)
#include "teststatuscommand.moc"

