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
#include <QtCore>
#include <algorithm>
#include <iostream>

namespace morgoth {

class ServerManagerPrivate {
public:
    QList<Server*> servers;
};

ServerManager::ServerManager(QObject* parent) :
    QObject(parent),
    d(new ServerManagerPrivate)
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

QStringList ServerManager::serverNames() const
{
    QStringList result;
    std::for_each(d->servers.begin(), d->servers.end(), [&result](const Server* s) {
        result.append(s->name());
    });

    return result;
}

} // namespace Morgoth
