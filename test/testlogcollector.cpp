#include <QtTest/QtTest>
#include "logcollector.h"

class TestLogCollector : public QObject {
    Q_OBJECT

    QString dir = QCoreApplication::applicationDirPath() + "/tmp_logcollector";

private slots:
    void initTestCase();
    void cleanupTestCase();

    void createLogDirectory();

};



void TestLogCollector::initTestCase()
{
    QDir().mkdir(dir);
}

void TestLogCollector::cleanupTestCase()
{
    QDir().rmdir(dir);
}

void TestLogCollector::createLogDirectory()
{
    QString logDir = dir + "/logs";
    morgoth::LogCollector lc(logDir);
    lc.log("foo");
    lc.save();
    QVERIFY(QDir(logDir).exists());
}

QTEST_MAIN(TestLogCollector)
#include "testlogcollector.moc"
