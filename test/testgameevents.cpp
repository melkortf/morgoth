#include <QtTest/QtTest>
#include "loglistener.h"
#include "playerinfo.h"
#include "gameevents/playerconnected.h"
#include "gameevents/playerdropped.h"
#include "gameevents/statushostname.h"
#include "gameevents/statusipaddress.h"
#include "gameevents/statusmap.h"
#include "gameevents/statusplayernumbers.h"

class TestGameEvents : public QObject {
    Q_OBJECT
    morgoth::LogListener* logListener = nullptr;

private slots:
    void init();
    void cleanup();

    void playerConnected();
    void playerDropped();

};

void TestGameEvents::init()
{
    Q_ASSERT(logListener == nullptr);

    QString fileName = QFINDTESTDATA("dm-console.log.20180306023605");
    QVERIFY(QFile(fileName).exists());

    logListener = new morgoth::LogListener(fileName);
    QVERIFY(logListener);
}

void TestGameEvents::cleanup()
{
    logListener->requestInterruption();
    logListener->wait();
    delete logListener;
    logListener = nullptr;
}

void TestGameEvents::playerConnected()
{
    morgoth::PlayerConnected* playerConnected = new morgoth::PlayerConnected;
    QSignalSpy spy(playerConnected, &morgoth::GameEvent::activated);
    logListener->installGameEvent(playerConnected);

    logListener->start();

    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);

    QCOMPARE(playerConnected->player().name(), "mały");
    QCOMPARE(playerConnected->player().steamId().toSteamId64(), 76561198074409147);
}

void TestGameEvents::playerDropped()
{
    morgoth::PlayerDropped* playerDropped = new morgoth::PlayerDropped;
    QSignalSpy spy(playerDropped, &morgoth::GameEvent::activated);
    logListener->installGameEvent(playerDropped);

    logListener->start();

    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);

    QCOMPARE(playerDropped->playerName(), "mały");
}

QTEST_MAIN(TestGameEvents)
#include "testgameevents.moc"
