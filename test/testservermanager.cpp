#include <QtTest>
#include "server.h"
#include "servermanager.h"

using namespace morgoth;

class TestServerManager: public QObject {
    Q_OBJECT

    ServerManager* serverManager;

private slots:
    void init();
    void cleanup();

    void reparentAddedServer();
    void find();
    void denyNameDuplicate();

};

void TestServerManager::init()
{
    serverManager = new ServerManager();
}

void TestServerManager::cleanup()
{
    delete serverManager;
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
}

void TestServerManager::denyNameDuplicate()
{
    Server* server = serverManager->add(QUrl::fromLocalFile("/some/path"), "test");
    QVERIFY(server != nullptr);

    Server* server2 = serverManager->add(QUrl::fromLocalFile("/some/other/path"), "test");
    QVERIFY(server2 == nullptr);
}

QTEST_MAIN(TestServerManager)
#include "testservermanager.moc"
