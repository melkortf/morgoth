#include "gameservermock.h"
#include "gameservermockadaptor.h"
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

void GameServerMock::setMap(const QString& map)
{
    m_map = map;
    emit mapChanged(map);
}

void GameServerMock::setMaxPlayers(int maxPlayers)
{
    m_maxPlayers = maxPlayers;
}

void GameServerMock::setAddress(const QString& address)
{
    m_address = address;
}
