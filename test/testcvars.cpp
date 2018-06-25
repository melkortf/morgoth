#include <QtTest/QtTest>
#include "loglistener.h"
#include "gameevents/cvarvalue.h"

class TestCvars : public QObject {
    Q_OBJECT
    morgoth::LogListener* logListener;

private slots:
    void init();
    void cleanup();

    void sv_password();
    void tv_password();

};

void TestCvars::init()
{
    QString fileName = QFINDTESTDATA("one-console.log.20180625000736");
    QVERIFY(QFile(fileName).exists());

    logListener = new morgoth::LogListener(fileName);
    QVERIFY(logListener);
}

void TestCvars::cleanup()
{
    logListener->requestInterruption();
    logListener->wait();
    delete logListener;
}

void TestCvars::sv_password()
{
    morgoth::CvarValue* cvar = new morgoth::CvarValue("sv_password");
    QSignalSpy spy(cvar, &morgoth::EventHandler::activated);
    logListener->installEventHandler(cvar);

    logListener->start();

    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);

    QCOMPARE(cvar->value(), "2ULBPW8KPvwxjSVy");
    QCOMPARE(cvar->defaultValue(), "");
}

void TestCvars::tv_password()
{
    morgoth::CvarValue* cvar = new morgoth::CvarValue("tv_password");
    QSignalSpy spy(cvar, &morgoth::EventHandler::activated);
    logListener->installEventHandler(cvar);

    logListener->start();

    QVERIFY(spy.wait());
    QCOMPARE(spy.count(), 1);

    QCOMPARE(cvar->value(), "");
    QCOMPARE(cvar->defaultValue(), "");
}

QTEST_MAIN(TestCvars)
#include "testcvars.moc"
