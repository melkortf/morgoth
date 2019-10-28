#include <QtTest/QtTest>
#include "server.h"
#include "servermanager.h"

using namespace morgoth;

class TestServerManager: public QObject {
    Q_OBJECT

    ServerManager* serverManager;

private slots:
    void initTestCase();
    void init();
    void cleanup();

    void emitServerAdded();
    void emitServerAboutToBeRemoved();
    void reparentAddedServer();
    void find();
    void denyNameDuplicate();
    void handleGameServerConnection();

};

void TestServerManager::initTestCase()
{
    QDBusConnection con = QDBusConnection::sessionBus();
    QVERIFY(con.isConnected());
}

void TestServerManager::init()
{
    serverManager = new ServerManager();
}

void TestServerManager::cleanup()
{
    delete serverManager;
}

void TestServerManager::emitServerAdded()
{
    Server* server = new Server(QUrl::fromLocalFile("/some/path"), "test");
    QSignalSpy spy(serverManager, &ServerManager::serverAdded);
    serverManager->add(server);
    QCOMPARE(spy.count(), 1);
    QVariantList arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).value<Server*>(), server);
}

void TestServerManager::emitServerAboutToBeRemoved()
{
    Server* server = new Server(QUrl::fromLocalFile("/some/path"), "test");
    serverManager->add(server);

    QSignalSpy spy(serverManager, &ServerManager::serverAboutToBeRemoved);
    serverManager->remove("test");
    QCOMPARE(spy.count(), 1);
    QVariantList arguments = spy.takeFirst();
    QCOMPARE(arguments.at(0).value<Server*>(), server);
}

void TestServerManager::reparentAddedServer()
{
    Server* server = new Server(QUrl::fromLocalFile("/some/path"), "test");
    serverManager->add(server);
    QVERIFY(server->parent() == serverManager);

    QObject* p = new QObject();
    Server* server2 = new Server(QUrl::fromLocalFile("/some/path"), "test2", p);
    QVERIFY(server2->parent() == p);
    serverManager->add(server2);
    QVERIFY(server2->parent() == serverManager);

    delete p;
}

void TestServerManager::find()
{
    Server* server = serverManager->add(QUrl::fromLocalFile("/some/path"), "test");
    QVERIFY(server);

    Server* server2 = serverManager->find("test");
    QVERIFY(server == server2);

    QVERIFY(serverManager->serverNames().contains("test"));
    QVERIFY(serverManager->servers().contains(server));
}

void TestServerManager::denyNameDuplicate()
{
    Server* server = serverManager->add(QUrl::fromLocalFile("/some/path"), "test");
    QVERIFY(server != nullptr);

    Server* server2 = serverManager->add(QUrl::fromLocalFile("/some/other/path"), "test");
    QVERIFY(server2 == nullptr);
}


void TestServerManager::handleGameServerConnection()
{
    QVERIFY(!serverManager->dbusServerAddress().isEmpty());

    Server* server = serverManager->add(QUrl::fromLocalFile("/some/path"), "test");
    QVERIFY(server);

    QSignalSpy spy(server, &Server::gameServerOnline);
    QVERIFY(spy.isValid());

    QProcess testingGameServer;
    testingGameServer.start(QStringLiteral("%1/utils/testinggameserver").arg(QCoreApplication::applicationDirPath()), {
        serverManager->dbusServerAddress(),
        QStringLiteral("/some/path")
    });

    QVERIFY(testingGameServer.waitForStarted());

    QVERIFY(spy.wait(1000));
    QCOMPARE(spy.count(), 1);
    auto arguments = spy.takeFirst();
    QVERIFY(arguments.at(0).isValid());

    QVERIFY(testingGameServer.waitForFinished());
}

QTEST_MAIN(TestServerManager)
#include "testservermanager.moc"
