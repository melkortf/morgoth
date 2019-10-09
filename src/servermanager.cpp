// This file is part of morgoth.

// morgoth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "servermanager.h"
#include "morgothdaemon.h"
#include "servermanageradaptor.h"
#include "serverstatus.h"
#include "gameserverinterface.h"
#include <QtCore>
#include <algorithm>
#include <iostream>

using org::morgoth::connector::GameServer;

namespace morgoth {

constexpr auto DBusServerAddress = "unix:path=/tmp/morgoth-server";

class ServerManagerPrivate {
    Q_DISABLE_COPY(ServerManagerPrivate)
    Q_DECLARE_PUBLIC(ServerManager)
    ServerManager *const q_ptr;

public:
    explicit ServerManagerPrivate(ServerManager* serverManager);

    void startDBusServer();
    void registerGameServer(const QDBusConnection& connection);

    QList<Server*> servers;
    QList<GameServer*> gameServers;
    QDBusServer* dbusServer = nullptr;

};


ServerManagerPrivate::ServerManagerPrivate(ServerManager* serverManager) :
    q_ptr(serverManager)
{
    startDBusServer();
}

void ServerManagerPrivate::startDBusServer()
{
    Q_Q(ServerManager);
    dbusServer = new QDBusServer(DBusServerAddress, q);

    if (dbusServer->isConnected()) {
        dbusServer->setAnonymousAuthenticationAllowed(true);

        QObject::connect(dbusServer, &QDBusServer::newConnection, [this](const QDBusConnection& connection) {
            registerGameServer(connection);
        });
    } else {
        qFatal("Failed to start the DBus server (%s)", DBusServerAddress);
    }
}

void ServerManagerPrivate::registerGameServer(const QDBusConnection& connection)
{
    Q_Q(ServerManager);
    GameServer* iface = new GameServer(QString(), "/", connection, q);
    QDir gameLocation = iface->gameLocation();

    auto it = std::find_if(servers.begin(), servers.end(), [&gameLocation](const Server* server) {
        return QDir(server->path().toLocalFile()) == gameLocation;
    });

    if (it != servers.end()) {
        qDebug("Server %s is online", qPrintable((*it)->name()));
        QObject::connect(iface, &GameServer::aboutToQuit, [this, iface]() {
            Q_ASSERT(gameServers.contains(iface));
            gameServers.removeAll(iface);
            iface->deleteLater();
        });

        emit (*it)->gameServerOnline(iface);
        gameServers.append(iface);
    } else {
        qWarning("Could not match %s to any of the known servers", qPrintable(iface->gameLocation()));
    }
}

ServerManager::ServerManager(QObject* parent) :
    QObject(parent),
    d_ptr(new ServerManagerPrivate(this))
{
    new ServerManagerAdaptor(this);
    if (morgothd)
        morgothd->dbusConnection().registerObject("/servers", this);
}

ServerManager::~ServerManager()
{

}

Server* ServerManager::find(const QString& name) const
{
    Q_D(const ServerManager);

    auto it = std::find_if(d->servers.begin(), d->servers.end(), [&name](auto s) {
        return s->name() == name;
    });
    return it == d->servers.end() ? nullptr : *it;
}

Server* ServerManager::add(const QUrl& path, const QString& name)
{
    QUrl fixedPath(path);
    if (QFile::exists(fixedPath.toString())) {
        fixedPath = QUrl::fromLocalFile(fixedPath.toString());
    }

    Server* s = new Server(fixedPath, name);
    if (add(s)) {
        return s;
    } else {
        delete s;
        return nullptr;
    }
}

bool ServerManager::add(Server* server)
{
    Q_D(ServerManager);

    if (d->servers.contains(server)) {
        qWarning("Could not add server \"%s\": server already added", qPrintable(server->name()));
        return false;
    }

    if (find(server->name()) != nullptr) {
        qWarning("Could not add server \"%s\": name taken", qPrintable(server->name()));
        return false;
    }

    server->setParent(this);
    d->servers.append(server);
    emit serverAdded(server);
    qInfo("Server %s added", qPrintable(server->name()));
    return true;
}

bool ServerManager::remove(const QString& serverName)
{
    Q_D(ServerManager);

    Server* server = find(serverName);
    if (server == nullptr) {
        qWarning("Could not find server \"%s\"; not removing anything", qPrintable(serverName));
        return false;
    }

    emit serverAboutToBeRemoved(server);
    d->servers.removeAll(server);
    server->deleteLater();
    qInfo("Server %s removed", qPrintable(serverName));
    return true;
}

QDBusObjectPath ServerManager::serverPath(const QString& serverName) const
{
    Server* server = find(serverName);
    if (server)
        return QDBusObjectPath(QStringLiteral("/servers/%1").arg(server->name()));
    else
        return QDBusObjectPath();
}

const QList<Server*>& ServerManager::servers() const
{
    Q_D(const ServerManager);
    return d->servers;
}

QStringList ServerManager::serverNames() const
{
    Q_D(const ServerManager);

    QStringList result;
    std::for_each(d->servers.begin(), d->servers.end(), [&result](const Server* s) {
        result.append(s->name());
    });

    return result;
}

QString ServerManager::dbusServerAddress() const
{
    Q_D(const ServerManager);
    return d->dbusServer->isConnected() ? d->dbusServer->address() : QString();
}

} // namespace Morgoth
