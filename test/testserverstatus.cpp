#include <QtTest/QtTest>
#include "servermanager.h"
#include "serverstatus.h"
#include "utils/testinggameservercontroller.h"

using namespace morgoth;

class TestServerStatus : public QObject {
    Q_OBJECT
    ServerManager* serverManager;
    Server* server;
    ServerStatus* serverStatus;

private slots:
    void initTestCase();
    void init();
    void cleanup();

    void emptyAllProps();
    void fetchProps();
};

void TestServerStatus::initTestCase()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());
}

void TestServerStatus::init()
{
    serverManager = new ServerManager();
    QVERIFY(!serverManager->dbusServerAddress().isEmpty());

    server = serverManager->add(QUrl::fromLocalFile("/some/path"), "test");
    QVERIFY(server);

    serverStatus = new ServerStatus(server);
}

void TestServerStatus::cleanup()
{
    delete serverStatus;
    delete serverManager;
}

void TestServerStatus::emptyAllProps()
{
    // verify all properties are empty/zeroed
    QCOMPARE(serverStatus->hostname(), QString());
    QCOMPARE(serverStatus->playerCount(), 0);
    QCOMPARE(serverStatus->maxPlayers(), 0);
    QCOMPARE(serverStatus->map(), QString());
    QCOMPARE(serverStatus->address(), QUrl());
    QCOMPARE(serverStatus->password(), QString());
    QCOMPARE(serverStatus->stvPort(), 0);
    QCOMPARE(serverStatus->stvPassword(), QString());
    QCOMPARE(serverStatus->players().size(), 0);
}

void TestServerStatus::fetchProps()
{
    TestingGameServerController controller;
    controller.setPath("/some/path");
    controller.start();
    controller.setMaxPlayers(7);
    controller.setAddress("127.0.0.1:27015");
    controller.connect(serverManager->dbusServerAddress());

    QTest::qWait(1000);

    QCOMPARE(serverStatus->maxPlayers(), 7);
    QCOMPARE(serverStatus->address(), QUrl("steam://127.0.0.1:27015"));

    controller.stop();
}

QTEST_MAIN(TestServerStatus)
#include "testserverstatus.moc"
