#include <QtTest/QtTest>
#include "server.h"
#include "servercoordinator.h"

using namespace morgoth;

class TestServerCoordinator: public QObject {
    Q_OBJECT

private slots:
    void create();
    void starting();

};

void TestServerCoordinator::create()
{
    Server* server = new Server(QUrl::fromLocalFile("/some/path"), "test");
    ServerCoordinator* coordinator = new ServerCoordinator(server);
    QCOMPARE(coordinator->server(), server);
    QCOMPARE(coordinator->state(), ServerCoordinator::State::Offline);
    QCOMPARE(coordinator->error(), ServerError::NoError);
    QVERIFY(!coordinator->sessionName().isEmpty());
}

void TestServerCoordinator::starting()
{
    Server* server = new Server(QUrl::fromLocalFile("/some/path"), "test");
    ServerCoordinator* coordinator = new ServerCoordinator(server);

    QSignalSpy spy(coordinator, &ServerCoordinator::stateChanged);
    coordinator->start();

    QCOMPARE(spy.count(), 2);

    QList<QVariant> arguments = spy.at(0);
    QCOMPARE(arguments.at(0).value<morgoth::ServerCoordinator::State>(), ServerCoordinator::Starting);

    arguments = spy.at(1);
    QCOMPARE(arguments.at(0).value<morgoth::ServerCoordinator::State>(), ServerCoordinator::Crashed);
    QCOMPARE(coordinator->error(), ServerError::InvalidInstallation);
}


QTEST_MAIN(TestServerCoordinator)
#include "testservercoordinator.moc"
