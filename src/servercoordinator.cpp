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
#include <QtCore>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

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
    void onGameServerStarted(org::morgoth::connector::GameServer* gameServer);
    void onGameServerStopped();
    void stopSync();

    Server* server;
    ServerCoordinator::State state = ServerCoordinator::State::Offline;
    TmuxSessionWrapper tmux;
    QString logFileName;
};

ServerCoordinatorPrivate::ServerCoordinatorPrivate(ServerCoordinator* coordinator, Server* server) :
    q_ptr(coordinator),
    server(server)
{
    QObject::connect(server, &Server::gameServerOnline, coordinator,
                     [this](org::morgoth::connector::GameServer* gameServer) { onGameServerStarted(gameServer); });
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
    Q_ASSERT(server->configuration());

    if (!server->isValid()) {
        qWarning("%s is not installed properly", qPrintable(server->name()));
        return false;
    }

    if (state != ServerCoordinator::Offline) {
        qWarning("%s is already running", qPrintable(server->name()));
        return false;
    }

    qInfo("%s: starting...", qPrintable(server->name()));
    setState(ServerCoordinator::Starting);

    QString user = server->configuration()->value("org.morgoth.Server.user");
    if (user.isEmpty() && morgothd)
        user = morgothd->config().value("user").toString();

    tmux.setUser(user);

    if (!tmux.create()) {
        qWarning("%s: could not create a tmux session", qPrintable(server->name()));
        setState(ServerCoordinator::Crashed);
        return false;
    }

    QString logDirectory = server->configuration()->value("org.morgoth.Server.logDirectory");
    if (QDir::isRelativePath(logDirectory))
        logDirectory.prepend(server->path().toLocalFile());

    logFileName = QString("%1/gameserver.log").arg(logDirectory);

    if (!user.isEmpty()) {
        passwd* pwd = ::getpwnam(user.toLocal8Bit().constData());
        if (!pwd) {
            char* error = ::strerror(errno);
            qWarning("Error retrieving uid and gid of user %s (%s)", qPrintable(user), error);
            setState(ServerCoordinator::Crashed);
            return false;
        } else {
            ::chown(logFileName.toLocal8Bit().constData(), pwd->pw_uid, pwd->pw_gid);
        }
    }

    if (!tmux.redirectOutput(logFileName)) {
        qWarning("%s: could not redirect output to %s", qPrintable(server->name()), qPrintable(logFileName));
        tmux.kill();
        setState(ServerCoordinator::Crashed);
        return false;
    }

    QString arguments = server->configuration()->value("org.morgoth.Server.launchArguments");
    QString cmd = QString("%1/srcds_run %2").arg(server->path().toLocalFile(), arguments);
    if (!tmux.sendKeys(cmd)) {
        qWarning("%s: could not start the server", qPrintable(server->name()));
        tmux.kill();
        setState(ServerCoordinator::Crashed);
        return false;
    }

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

void ServerCoordinatorPrivate::onGameServerStarted(org::morgoth::connector::GameServer* gameServer)
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
        qWarning("Server %s is stopping, but it was NOT registered as running!", qPrintable(server->name()));
    }

    setState(ServerCoordinator::Offline);
    qInfo("%s: stopped", qPrintable(server->name()));
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
    Q_ASSERT(server->isValid());

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

static void registerMetaType()
{
    qDBusRegisterMetaType<morgoth::ServerCoordinator::State>();
}
Q_COREAPP_STARTUP_FUNCTION(registerMetaType)
