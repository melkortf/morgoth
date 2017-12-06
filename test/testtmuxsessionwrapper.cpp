#include <QtTest/QtTest>
#include "tmuxsessionwrapper.h"

class TestTmuxSessionWrapper : public QObject {
    Q_OBJECT

    QString tmuxExec;
    QRegularExpression nameRegex;

private slots:
    void initTestCase();

    void generateName();
    void testNotCreated();
    void testCreate();
    void testCleanup();

};


void TestTmuxSessionWrapper::initTestCase()
{
    tmuxExec = "/usr/bin/tmux";
    Q_ASSERT(QFile(tmuxExec).exists());

    nameRegex = QRegularExpression("^[A-Za-z0-9\\-]+$");
}

void TestTmuxSessionWrapper::generateName()
{
    morgoth::TmuxSessionWrapper tmux;
    QString name = tmux.name();
    QVERIFY(nameRegex.match(name).hasMatch());
}

void TestTmuxSessionWrapper::testNotCreated()
{
    morgoth::TmuxSessionWrapper tmux;
    bool result = tmux.exists();
    QCOMPARE(result, false);
    result = tmux.kill();
    QCOMPARE(result, true);
    result = tmux.redirectOutput("whatever");
    QCOMPARE(result, false);
    result = tmux.sendKeys("another whatever");
    QCOMPARE(result, false);
}

void TestTmuxSessionWrapper::testCreate()
{
    morgoth::TmuxSessionWrapper tmux;
    bool result = tmux.create();
    QVERIFY(result);
    QVERIFY(tmux.exists());

    QString name = tmux.name();
    QProcess p;
    p.start(tmuxExec, { "has-session", "-t", name });
    p.waitForFinished();
    QVERIFY(p.exitCode() == 0);

    tmux.kill();
}

void TestTmuxSessionWrapper::testCleanup()
{
    QString name;
    {
        morgoth::TmuxSessionWrapper tmux;
        QVERIFY(tmux.create());
        QVERIFY(tmux.exists());
        name = tmux.name();
    }

    QProcess p;
    p.start(tmuxExec, { "has-session", "-t", name });
    p.waitForFinished();
    QVERIFY(p.exitCode() != 0);
}

QTEST_MAIN(TestTmuxSessionWrapper)
#include "testtmuxsessionwrapper.moc"
