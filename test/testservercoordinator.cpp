#include <QtTest/QtTest>
#include "server.h"
#include "servercoordinator.h"

using namespace morgoth;

class TestServerCoordinator: public QObject {
    Q_OBJECT

private slots:
    void create();

};

void TestServerCoordinator::create()
{
    Server* server = new Server(QUrl::fromLocalFile("/some/path"), "test");
    ServerCoordinator* coordinator = new ServerCoordinator(server);
    QCOMPARE(coordinator->server(), server);
    QCOMPARE(coordinator->state(), ServerCoordinator::State::Offline);
    QCOMPARE(coordinator->error(), ServerCoordinator::Error::NoError);
    QVERIFY(!coordinator->sessionName().isEmpty());
}


QTEST_MAIN(TestServerCoordinator)
#include "testservercoordinator.moc"
