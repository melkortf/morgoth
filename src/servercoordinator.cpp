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

#include "servercoordinator.h"
#include "gameserverinterface.h"
#include "morgothdaemon.h"
#include "serverconfiguration.h"
#include "servercoordinatoradaptor.h"
#include "serverstarter.h"
#include <QtCore>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

using org::morgoth::connector::GameServer;

// The time we wait for the game server to become online after starting it
// 30 seconds
static constexpr auto GameServerStartTimeout = 30 * 1000;

namespace morgoth {

class ServerCoordinatorPrivate {
    Q_DISABLE_COPY(ServerCoordinatorPrivate)
    Q_DECLARE_PUBLIC(ServerCoordinator)
    ServerCoordinator *const q_ptr;

public:
    explicit ServerCoordinatorPrivate(ServerCoordinator* coordinator, Server* server);

    void setState(ServerCoordinator::State state);
    bool start();
    void stop();
    void onGameServerStarted(GameServer* gameServer);
    void onGameServerStopped();
    void onGameServerTimedOut();
    void onServerFailed(ServerError error);
    void stopSync();

    Server* server;
    ServerCoordinator::State state = ServerCoordinator::State::Offline;
    ServerError error;
    TmuxSessionWrapper tmux;
    QString logFileName;
};

ServerCoordinatorPrivate::ServerCoordinatorPrivate(ServerCoordinator* coordinator, Server* server) :
    q_ptr(coordinator),
    server(server)
{
    QObject::connect(server, &Server::gameServerTimedOut, coordinator,
                     [this]() { onGameServerTimedOut(); });
    QObject::connect(qApp, &QCoreApplication::aboutToQuit, coordinator,
                     [this]() { stopSync(); });
}

void ServerCoordinatorPrivate::setState(ServerCoordinator::State state)
{
    Q_Q(ServerCoordinator);
    this->state = state;
    emit q->stateChanged(this->state);
}

bool ServerCoordinatorPrivate::start()
{
    ServerStarter* starter = new ServerStarter(server, &tmux);
    QObject::connect(starter, &ServerStarter::starting, std::bind(&ServerCoordinatorPrivate::setState, this, ServerCoordinator::Starting));
    QObject::connect(starter, &ServerStarter::started, std::bind(&ServerCoordinatorPrivate::onGameServerStarted, this, std::placeholders::_1));
    QObject::connect(starter, &ServerStarter::started, starter, &QObject::deleteLater);
    QObject::connect(starter, &ServerStarter::failed, std::bind(&ServerCoordinatorPrivate::onServerFailed, this, std::placeholders::_1));
    QObject::connect(starter, &ServerStarter::failed, starter, &QObject::deleteLater);
    starter->start();
    return true;
}

void ServerCoordinatorPrivate::stop()
{
    if (state == ServerCoordinator::Running || state == ServerCoordinator::Starting) {
        qDebug("%s: stopping...", qPrintable(server->name()));
        setState(ServerCoordinator::ShuttingDown);
        tmux.sendKeys("quit");
    }
}

void ServerCoordinatorPrivate::onGameServerStarted(GameServer* gameServer)
{
    Q_Q(ServerCoordinator);

    if (state != ServerCoordinator::Starting) {
        qWarning("Server located at %s was NOT started by morgoth!", qPrintable(gameServer->gameLocation()));
    }

    QObject::connect(gameServer, &org::morgoth::connector::GameServer::aboutToQuit, q, [this]() {
        onGameServerStopped();
    });

    setState(ServerCoordinator::Running);
    qInfo("%s: started", qPrintable(server->name()));
}

void ServerCoordinatorPrivate::onGameServerStopped()
{
    if (state != ServerCoordinator::ShuttingDown) {
        qWarning("Server %s is stopping, but it was NOT marked for shut down!", qPrintable(server->name()));
    }

    setState(ServerCoordinator::Offline);

    if (!tmux.kill()) {
        qWarning("Could not kill session %s", qPrintable(tmux.name()));
    }

    qInfo("%s: stopped", qPrintable(server->name()));
}

void ServerCoordinatorPrivate::onGameServerTimedOut()
{
    setState(ServerCoordinator::Crashed);

    if (!tmux.kill()) {
        qWarning("Could not kill session %s", qPrintable(tmux.name()));
    }

    qWarning("%s: crashed", qPrintable(server->name()));
}

void ServerCoordinatorPrivate::onServerFailed(ServerError error)
{
    this->error = error;
    setState(ServerCoordinator::Crashed);
}

void ServerCoordinatorPrivate::stopSync()
{
    // TODO
    // send stop keys and wait for the server to actually shut down
}


ServerCoordinator::ServerCoordinator(Server* server) :
    QObject(server),
    d_ptr(new ServerCoordinatorPrivate(this, server))
{
    new ServerCoordinatorAdaptor(this);
    if (morgothd)
        morgothd->dbusConnection().registerObject(server->coordinatorPath().path(), this);
}

ServerCoordinator::~ServerCoordinator()
{
    // stopSync() cannot be used here, as the parent Server instance
    // is already destroyed and we relay on its existence
}

void ServerCoordinator::sendCommand(const QString& command)
{
    Q_D(ServerCoordinator);

    if (d->state == Running)
        d->tmux.sendKeys(command);
}

const Server* ServerCoordinator::server() const
{
    Q_D(const ServerCoordinator);
    return d->server;
}

ServerCoordinator::State ServerCoordinator::state() const
{
    Q_D(const ServerCoordinator);
    return d->state;
}

ServerError ServerCoordinator::error() const
{
    Q_D(const ServerCoordinator);
    return d->error;
}

QString ServerCoordinator::sessionName() const
{
    Q_D(const ServerCoordinator);
    return d->tmux.name();
}

bool ServerCoordinator::start()
{
    Q_D(ServerCoordinator);
    return d->start();
}

void ServerCoordinator::stop()
{
    Q_D(ServerCoordinator);
    d->stop();
}

} // namespace Morgoth

QDBusArgument& operator<<(QDBusArgument& argument, const morgoth::ServerCoordinator::State& state)
{
    QString strStatus = QMetaEnum::fromType<morgoth::ServerCoordinator::State>().valueToKey(state);

    argument.beginStructure();
    argument << strStatus;
    argument.endStructure();

    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, morgoth::ServerCoordinator::State& state)
{
    QString strStatus;

    argument.beginStructure();
    argument >> strStatus;
    argument.endStructure();

    int value = QMetaEnum::fromType<morgoth::ServerCoordinator::State>().keyToValue(strStatus.toLocal8Bit().constData());
    state = static_cast<morgoth::ServerCoordinator::State>(value);
    return argument;
}

static void registerMetaTypes()
{
    qDBusRegisterMetaType<morgoth::ServerCoordinator::State>();
}
Q_COREAPP_STARTUP_FUNCTION(registerMetaTypes)
