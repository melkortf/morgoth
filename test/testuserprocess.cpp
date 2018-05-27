#include <QtTest/QtTest>
#include "userprocess.h"

class TestUserProcess : public QObject {
    Q_OBJECT

private slots:
    void resolveRoot();

};

void TestUserProcess::resolveRoot()
{
    morgoth::UserProcess userProcess;
    userProcess.setUser("root");

    QCOMPARE(userProcess.user(), "root");
    QCOMPARE(userProcess.groupId(), 0);
    QCOMPARE(userProcess.userId(), 0);
}

QTEST_MAIN(TestUserProcess)
#include "testuserprocess.moc"
