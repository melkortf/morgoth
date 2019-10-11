#include <QtTest/QtTest>
#include "server.h"
#include "servermanager.h"

using namespace morgoth;

class TestServerManager: public QObject {
    Q_OBJECT

    ServerManager* serverManager;

private slots:
    void init();
    void cleanup();

    void emitServerAdded();
    void emitServerAboutToBeRemoved();
    void reparentAddedServer();
    void find();
    void denyNameDuplicate();
    void handleGameServerConnection();

};

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

class GameServerStub : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString gameLocation READ gameLocation)

public:
    void connect(const QString& address)
    {
        QDBusConnection connection = QDBusConnection::connectToPeer(address, "morgoth-server");
        QVERIFY(connection.isConnected());
        bool registered = connection.registerObject("/", this);
        QVERIFY(registered);
    }

    QString gameLocation() const
    {
        return "/some/path";
    }

};

void TestServerManager::handleGameServerConnection()
{
    QVERIFY(!serverManager->dbusServerAddress().isEmpty());

    Server* server = serverManager->add(QUrl::fromLocalFile("/some/path"), "test");
    QVERIFY(server);

    QSignalSpy spy(server, SIGNAL(gameServerOnline(org::morgoth::connector::GameServer*)));

    GameServerStub* gameServer = new GameServerStub();
    gameServer->connect(serverManager->dbusServerAddress());
    QCoreApplication::processEvents(QEventLoop::AllEvents | QEventLoop::WaitForMoreEvents);
    QEXPECT_FAIL("", "Pending investigation why the signal isn't being emitted", Continue);
    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(TestServerManager)
#include "testservermanager.moc"
