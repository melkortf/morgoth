#include <QtTest/QtTest>
#include "loglistener.h"
#include "gameevents/statushostname.h"
#include "gameevents/statusmap.h"
#include "gameevents/statusplayernumbers.h"

class TestGameEvents : public QObject {
    Q_OBJECT

    morgoth::LogListener* logListener;

private slots:
    void init();
    void cleanup();

    void statusHostname();
    void playerNumbers();
    void map();

};



void TestGameEvents::init()
{
    QString fileName = QFINDTESTDATA("cmd_status.log");
    QVERIFY(QFile(fileName).exists());

    logListener = new morgoth::LogListener(fileName);
}

void TestGameEvents::cleanup()
{
    logListener->requestInterruption();
    logListener->wait();
}

void TestGameEvents::statusHostname()
{
    QVERIFY(logListener);

    morgoth::StatusHostname* hostnameLine = new morgoth::StatusHostname;
    QSignalSpy spy(hostnameLine, &morgoth::EventHandler::activated);
    logListener->installEventHandler(hostnameLine);

    logListener->start();
    spy.wait();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(hostnameLine->hostname(), "melkor.tf #1");
}

void TestGameEvents::playerNumbers()
{
    QVERIFY(logListener);

    morgoth::StatusPlayerNumbers* playerLine = new morgoth::StatusPlayerNumbers;
    QSignalSpy spy(playerLine, &morgoth::EventHandler::activated);
    logListener->installEventHandler(playerLine);

    logListener->start();
    spy.wait();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(playerLine->playerCount(), 0);
    QCOMPARE(playerLine->maxPlayers(), 25);
}

void TestGameEvents::map()
{
    QVERIFY(logListener);

    morgoth::StatusMap* mapLine = new morgoth::StatusMap;
    QSignalSpy spy(mapLine, &morgoth::EventHandler::activated);
    logListener->installEventHandler(mapLine);

    logListener->start();
    spy.wait();
    QCOMPARE(spy.count(), 1);
    QCOMPARE(mapLine->map(), "cp_process_final");
}

QTEST_MAIN(TestGameEvents)
#include "testgameevents.moc"
