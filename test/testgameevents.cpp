#include <QtTest/QtTest>
#include "loglistener.h"
#include "gameevents/playerconnected.h"
#include "gameevents/playerdropped.h"
#include "gameevents/statushostname.h"
#include "gameevents/statusipaddress.h"
#include "gameevents/statusmap.h"
#include "gameevents/statusplayernumbers.h"

class TestGameEvents : public QObject {
    Q_OBJECT

private slots:
    void playerConnected();

};

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
