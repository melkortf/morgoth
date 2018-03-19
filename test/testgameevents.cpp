#include <QtTest/QtTest>
#include "loglistener.h"
#include "gameevents/playerconnected.h"
#include "gameevents/playerdropped.h"
#include "gameevents/statushostname.h"
#include "gameevents/statusmap.h"
#include "gameevents/statusplayernumbers.h"

class TestGameEvents : public QObject {
    Q_OBJECT

private slots:
    void status();
    void playerConnected();

};


void TestGameEvents::status()
{
    QString fileName = QFINDTESTDATA("cmd_status.log");
    QVERIFY(QFile(fileName).exists());

    morgoth::LogListener* logListener = new morgoth::LogListener(fileName);
    QVERIFY(logListener);

    morgoth::StatusHostname* hostnameLine = new morgoth::StatusHostname;
    QSignalSpy spyHostname(hostnameLine, &morgoth::EventHandler::activated);
    logListener->installEventHandler(hostnameLine);

    morgoth::StatusPlayerNumbers* playerLine = new morgoth::StatusPlayerNumbers;
    QSignalSpy spyPlayers(playerLine, &morgoth::EventHandler::activated);
    logListener->installEventHandler(playerLine);

    morgoth::StatusMap* mapLine = new morgoth::StatusMap;
    QSignalSpy spyMap(mapLine, &morgoth::EventHandler::activated);
    logListener->installEventHandler(mapLine);

    logListener->start();

    spyHostname.wait(1000);
    QCOMPARE(spyHostname.count(), 1);
    QCOMPARE(hostnameLine->hostname(), "melkor.tf #1");

    spyPlayers.wait(100);
    QCOMPARE(spyPlayers.count(), 1);
    QCOMPARE(playerLine->playerCount(), 0);
    QCOMPARE(playerLine->maxPlayers(), 25);

    spyMap.wait(10);
    QCOMPARE(spyMap.count(), 1);
    QCOMPARE(mapLine->map(), "cp_process_final");

    logListener->requestInterruption();
    logListener->wait();
}

void TestGameEvents::playerConnected()
{
    QString fileName = QFINDTESTDATA("dm-console.log.20180306023605");
    QVERIFY(QFile(fileName).exists());

    morgoth::LogListener* logListener = new morgoth::LogListener(fileName);
    QVERIFY(logListener);

    morgoth::PlayerConnected* playerConnected = new morgoth::PlayerConnected;
    QSignalSpy spyConnected(playerConnected, &morgoth::EventHandler::activated);
    logListener->installEventHandler(playerConnected);

    morgoth::PlayerDropped* playerDropped = new morgoth::PlayerDropped;
    QSignalSpy spyDropped(playerDropped, &morgoth::EventHandler::activated);
    logListener->installEventHandler(playerDropped);

    logListener->start();

    QVERIFY(spyConnected.wait());
    QCOMPARE(spyConnected.count(), 1);

    spyDropped.wait(100);
    QCOMPARE(spyDropped.count(), 1);

    logListener->requestInterruption();
    logListener->wait();
}

QTEST_MAIN(TestGameEvents)
#include "testgameevents.moc"
