#include <QtTest/QtTest>
#include "server.h"

class TestServer : public QObject {
    Q_OBJECT

private slots:
    void handleInvalidServerInstallation();
    void denyRemoteServers();

};


void TestServer::handleInvalidServerInstallation()
{
    morgoth::Server* server = new morgoth::Server(QUrl::fromLocalFile("/non/existent/path"), "test");
    QCOMPARE(server->isValid(), false);
    QCOMPARE(server->coordinator(), nullptr);
    QCOMPARE(server->coordinatorPath(), QDBusObjectPath());
    QCOMPARE(server->status(), nullptr);
    QCOMPARE(server->statusPath(), QDBusObjectPath());
    QVERIFY(server->configuration() != nullptr);
    QCOMPARE(server->configurationPath(), QDBusObjectPath(QStringLiteral("/servers/test/configuration")));
}

void TestServer::denyRemoteServers()
{
    morgoth::Server* server = new morgoth::Server(QUrl("foo.bar:12345"), "test");
    QCOMPARE(server->isValid(), false);
    QCOMPARE(server->coordinator(), nullptr);
    QCOMPARE(server->coordinatorPath(), QDBusObjectPath());
    QCOMPARE(server->status(), nullptr);
    QCOMPARE(server->statusPath(), QDBusObjectPath());
    QVERIFY(server->configuration() != nullptr);
    QCOMPARE(server->configurationPath(), QDBusObjectPath(QStringLiteral("/servers/test/configuration")));
}

QTEST_MAIN(TestServer)
#include "testserver.moc"
