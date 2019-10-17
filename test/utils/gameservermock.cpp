#include "gameservermock.h"
#include "gameserveradaptor.h"
#include <QtDBus/QtDBus>
#include <QtTest/QtTest>

GameServerMock::GameServerMock(QObject* parent) :
    QObject(parent)
{
    new GameServerAdaptor(this);
}

void GameServerMock::connect(const QString& address)
{
    QDBusConnection connection = QDBusConnection::connectToPeer(address, "morgoth-server");
    Q_ASSERT(connection.isConnected());
    bool registered = connection.registerObject("/", this);
    Q_ASSERT(registered);

    qDebug() << "Testing game server connected to" << address;
}

const QString& GameServerMock::gameLocation() const
{
    qDebug() << Q_FUNC_INFO;
    return m_gameLocation;
}
