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

#include "server.h"
#include "morgothdaemon.h"
#include "servercoordinator.h"
#include "serveradaptor.h"
#include "serverstatus.h"
#include <QtCore>

namespace morgoth {

class ServerPrivate{
public:
    explicit ServerPrivate(Server* server, const QUrl& path, const QString& name);
    void discover();
    void initializeDefaultConfiguration();

    Server* d;
    QString name;
    QUrl path;
    bool valid;
    QString srcdsExec;
    ServerCoordinator* coordinator = nullptr;
    QDBusObjectPath coordinatorPath;
    ServerConfiguration* configuration = nullptr;
    QDBusObjectPath configurationPath;
    ServerStatus* status = nullptr;
    QDBusObjectPath statusPath;
};

ServerPrivate::ServerPrivate(Server* server, const QUrl& path, const QString& name) :
    d(server),
    name(name),
    path(path)
{

}

void ServerPrivate::discover()
{
    valid = false;

    if (!path.isLocalFile()) {
        qWarning("Cannot handle server %s: remote servers not supported", qPrintable(path.toString()));
        return;
    }

    srcdsExec = path.toLocalFile() + QDir::separator() + "srcds_run";

    if (!QFile::exists(srcdsExec)) {
        qWarning("%s does not exist", qPrintable(srcdsExec));
        return;
    }

    valid = true;
}

void ServerPrivate::initializeDefaultConfiguration()
{
    configuration = new ServerConfiguration(d);
    configuration->setValue("org.morgoth.Server.launchArguments", "-port 27015 -secured +map cp_badlands");
    configuration->setValue("org.morgoth.Server.logDirectory", "logs"); // relative to path
}

Server::Server(const QUrl& path, const QString& name, QObject* parent) :
    QObject(parent),
    d(new ServerPrivate(this, path, name))
{
    d->discover();

    new ServerAdaptor(this);

    if (morgothd) {
        QString dbusPath = QStringLiteral("/servers/%1").arg(name);
        morgothd->dbusConnection().registerObject(dbusPath, this);
    }

    d->configurationPath.setPath(QStringLiteral("/servers/%1/configuration").arg(name));
    d->initializeDefaultConfiguration();

    if (isValid()) {
        d->coordinatorPath.setPath(QStringLiteral("/servers/%1/coordinator").arg(name));
        d->coordinator = new ServerCoordinator(this);
        d->statusPath.setPath(QStringLiteral("/servers/%1/status").arg(name));
        d->status = new ServerStatus(d->coordinator, this);
    }
}

Server::~Server() {}

const QString& Server::srcdsExec()
{
    return d->srcdsExec;
}

const QString& Server::name() const
{
    return d->name;
}

const QUrl& Server::path() const
{
    return d->path;
}

bool Server::isValid() const
{
    return d->valid;
}

ServerCoordinator* Server::coordinator()
{
    return d->coordinator;
}

const ServerCoordinator* Server::coordinator() const
{
    return d->coordinator;
}

const QDBusObjectPath& Server::coordinatorPath()
{
    return d->coordinatorPath;
}

ServerConfiguration* Server::configuration()
{
    return d->configuration;
}

const ServerConfiguration* Server::configuration() const
{
    return d->configuration;
}

const QDBusObjectPath& Server::configurationPath() const
{
    return d->configurationPath;
}

ServerStatus* Server::status()
{
    return d->status;
}

const ServerStatus* Server::status() const
{
    return d->status;
}

const QDBusObjectPath& Server::statusPath() const
{
    return d->statusPath;
}

} // namespace Morgoth
