#include "testinggameservercontroller.h"
#include <QtCore>
#include <QtTest/QtTest>

TestingGameServerController::TestingGameServerController()
{

}

TestingGameServerController::~TestingGameServerController()
{

}

void TestingGameServerController::start()
{
    Q_ASSERT(!m_path.isEmpty());

    m_process.setProcessChannelMode(QProcess::ForwardedChannels);
    m_process.start(
        QStringLiteral("%1/utils/testinggameserver").arg(QCoreApplication::applicationDirPath()),
        { m_path }
    );
    QVERIFY(m_process.waitForStarted());
}

void TestingGameServerController::connect(const QString& dbusServerAddress)
{
    Q_ASSERT(m_process.state() == QProcess::Running);
    m_process.write(QStringLiteral("connect %1\n").arg(dbusServerAddress).toLocal8Bit());
    QVERIFY(QTest::qWaitFor([this]() { return m_process.bytesToWrite() == 0; }));
    qDebug() << "DUPA";
}

void TestingGameServerController::stop()
{
    m_process.write("quit\n");
    QVERIFY(m_process.waitForFinished());
}

void TestingGameServerController::setPath(const QString& path)
{
    m_path = path;
}

void TestingGameServerController::setMaxPlayers(int maxPlayers)
{
    Q_ASSERT(m_process.state() == QProcess::Running);
    m_process.write(QStringLiteral("set_maxplayers %1\n").arg(maxPlayers).toLocal8Bit());
    QVERIFY(QTest::qWaitFor([this]() { return m_process.bytesToWrite() == 0; }));
}

void TestingGameServerController::setAddress(const QString& address)
{
    Q_ASSERT(m_process.state() == QProcess::Running);
    m_process.write(QStringLiteral("set_address %1\n").arg(address).toLocal8Bit());
    QVERIFY(QTest::qWaitFor([this]() { return m_process.bytesToWrite() == 0; }));
}
