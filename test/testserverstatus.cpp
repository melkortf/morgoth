#include <QtTest/QtTest>
#include "serverstatus.h"

using namespace morgoth;

class TestServerStatus : public QObject {
    Q_OBJECT

private slots:
    void create();
    
};

void TestServerStatus::create()
{
    Server* server = new Server(QUrl::fromLocalFile("/some/path"), "test");
    ServerStatus status(server);

    // verify all values are empty/zeroed
    QCOMPARE(status.hostname(), QString());
    QCOMPARE(status.playerCount(), 0);
    QCOMPARE(status.maxPlayers(), 0);
    QCOMPARE(status.map(), QString());
    QCOMPARE(status.address(), QUrl());
    QCOMPARE(status.password(), QString());
    QCOMPARE(status.stvPort(), 0);
    QCOMPARE(status.stvPassword(), QString());
    QCOMPARE(status.players().size(), 0);
}

QTEST_MAIN(TestServerStatus)
#include "testserverstatus.moc"
