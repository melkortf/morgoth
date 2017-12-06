#include <QtTest/QtTest>
#include "server.h"

class TestServer : public QObject {
    Q_OBJECT

private slots:
    void handleInvalidServerInstallation();

};


void TestServer::handleInvalidServerInstallation()
{
    morgoth::Server* server = new morgoth::Server(QUrl::fromLocalFile("/non/existent/path"), "test");
    QCOMPARE(server->isValid(), false);
    QVERIFY(server->configuration() != nullptr);
    QVERIFY(server->coordinator() == nullptr);
}

QTEST_MAIN(TestServer)
#include "testserver.moc"
