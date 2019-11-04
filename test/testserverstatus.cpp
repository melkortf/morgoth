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
    void changeMap();
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

    QSignalSpy serverOnline(server, &Server::gameServerOnline);
    controller.connect(serverManager->dbusServerAddress());
    QVERIFY(serverOnline.wait());

    QCOMPARE(serverStatus->maxPlayers(), 7);
    QCOMPARE(serverStatus->address(), QUrl("steam://127.0.0.1:27015"));

    controller.stop();
}

void TestServerStatus::changeMap()
{
    TestingGameServerController controller;
    controller.setPath("/some/path");
    controller.start();

    QSignalSpy serverOnline(server, &Server::gameServerOnline);
    controller.connect(serverManager->dbusServerAddress());
    QVERIFY(serverOnline.wait());

    QSignalSpy spy(serverStatus, &ServerStatus::mapChanged);
    QVERIFY(spy.isValid());

    QCOMPARE(serverStatus->map(), QString());
    controller.setMap("cp_test_v1");
    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);
    auto arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).isValid());
    QCOMPARE(arguments.at(0).type(), QVariant::String);
    QCOMPARE(arguments.at(0).value<QString>(), "cp_test_v1");

    controller.stop();

}

QTEST_MAIN(TestServerStatus)
#include "testserverstatus.moc"
