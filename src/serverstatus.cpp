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

#include "serverstatus.h"
#include "eventhandler.h"
#include "morgothdaemon.h"
#include "serverstatusadaptor.h"
#include "gameevents/cvarvalue.h"
#include "gameevents/playerconnected.h"
#include "gameevents/playerdropped.h"
#include "gameevents/statushostname.h"
#include "gameevents/statusipaddress.h"
#include "gameevents/statusmap.h"
#include "gameevents/statusplayernumbers.h"
#include <QtCore>

namespace morgoth {

class ServerStatusPrivate {
public:
    explicit ServerStatusPrivate(ServerCoordinator* coordinator) :
        coordinator(coordinator) {}

    ServerCoordinator* coordinator;

    QString hostname;
    int playerCount = 0;
    int maxPlayers = 0;
    QString map;
    QUrl address;
    bool passwordProtected = false;
};

ServerStatus::ServerStatus(ServerCoordinator* coordinator, QObject* parent) :
    QObject(parent),
    d(new ServerStatusPrivate(coordinator))
{
    connect(coordinator, &ServerCoordinator::stateChanged, this, &ServerStatus::handleStateChange);
    initialize();

    new ServerStatusAdaptor(this);
    if (morgothd)
        morgothd->dbusConnection().registerObject(coordinator->server()->statusPath().path(), this);
}

ServerStatus::~ServerStatus()
{

}

const QString& ServerStatus::hostname() const
{
    return d->hostname;
}

int ServerStatus::playerCount() const
{
    return d->playerCount;
}

int ServerStatus::maxPlayers() const
{
    return d->maxPlayers;
}

QString ServerStatus::map() const
{
    return d->map;
}

QUrl ServerStatus::address() const
{
    return d->address;
}

bool ServerStatus::isPasswordProtected() const
{
    return d->passwordProtected;
}

void ServerStatus::initialize()
{
    // set up log listeners
    StatusHostname* hostnameLine = new StatusHostname;
    connect(hostnameLine, &EventHandler::activated, [hostnameLine, this]() {
        setHostname(hostnameLine->hostname());
    });
    d->coordinator->installEventHandler(hostnameLine);

    StatusPlayerNumbers* playerLine = new StatusPlayerNumbers;
    connect(playerLine, &EventHandler::activated, [playerLine, this]() {
        setPlayerCount(playerLine->playerCount());
        setMaxPlayers(playerLine->maxPlayers());
    });
    d->coordinator->installEventHandler(playerLine);

    StatusMap* mapLine = new StatusMap;
    connect(mapLine, &EventHandler::activated, [mapLine, this]() {
        setMap(mapLine->map());
    });
    d->coordinator->installEventHandler(mapLine);

    StatusIpAddress* ipLine = new StatusIpAddress;
    connect(ipLine, &EventHandler::activated, [ipLine, this]() {
        QUrl address;
        address.setScheme("steam");
        address.setHost(ipLine->ip());
        address.setPort(static_cast<int>(ipLine->port()));
        setAddress(address);
    });
    d->coordinator->installEventHandler(ipLine);

    PlayerConnected* playerConnected = new PlayerConnected;
    connect(playerConnected, &EventHandler::activated, [this]() {
        setPlayerCount(playerCount() + 1);
    });
    d->coordinator->installEventHandler(playerConnected);

    PlayerDropped* playerDropped = new PlayerDropped;
    connect(playerDropped, &EventHandler::activated, [this]() {
        setPlayerCount(playerCount() - 1);
    });
    d->coordinator->installEventHandler(playerDropped);

    CvarValue* password = new CvarValue("sv_password");
    connect(password, &EventHandler::activated, [password, this]() {
        setPasswordProtected(!password->value().isEmpty());
    });
    d->coordinator->installEventHandler(password);
}

void ServerStatus::reset()
{
    setHostname(QString());
    setPlayerCount(0);
    setMaxPlayers(0);
    setMap(QString());
    setAddress(QUrl());
    setPasswordProtected(false);
}

void ServerStatus::setHostname(const QString& hostname)
{
    d->hostname = hostname;
    emit hostnameChanged(d->hostname);
}

void ServerStatus::setPlayerCount(int playerCount)
{
    d->playerCount = playerCount;
    emit playerCountChanged(d->playerCount);
}

void ServerStatus::setMaxPlayers(int maxPlayers)
{
    d->maxPlayers = maxPlayers;
    emit maxPlayersChanged(d->maxPlayers);
}

void ServerStatus::setMap(const QString& map)
{
    d->map = map;
    emit mapChanged(d->map);
}

void ServerStatus::setAddress(const QUrl& address)
{
    d->address = address;
    emit addressChanged(d->address);
    emit addressChanged(d->address.toString());
}

void ServerStatus::setPasswordProtected(bool passwordProtected)
{
    d->passwordProtected = passwordProtected;
    emit passwordProtectedChanged(d->passwordProtected);
}

void ServerStatus::handleStateChange(ServerCoordinator::State serverState)
{
    switch (serverState) {
        case ServerCoordinator::State::Running:
            QTimer::singleShot(0, this, &ServerStatus::refreshStatus);
            break;

        default:
            reset();
            break;
    }
}

void ServerStatus::refreshStatus()
{
    // FIXME Execute rcon command instead of this
    d->coordinator->sendCommand("status");
    d->coordinator->sendCommand("sv_password");
}

} // namespace morgoth
