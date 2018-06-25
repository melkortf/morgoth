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
#include "gameevent.h"
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
#include <functional>

namespace morgoth {

class ServerStatusPrivate {
public:
    explicit ServerStatusPrivate(ServerStatus* q, ServerCoordinator* coordinator) :
        q(q), coordinator(coordinator) {}

    ServerStatus* q;
    ServerCoordinator* coordinator;

    QString hostname;
    int playerCount = 0;
    int maxPlayers = 0;
    QString map;
    QUrl address;
    bool passwordProtected = false;

    void initialize();
    void reset();
    void setHostname(const QString& hostname);
    void setPlayerCount(int playerCount);
    void setMaxPlayers(int maxPlayers);
    void setMap(const QString& map);
    void setAddress(const QUrl& address);
    void setPasswordProtected(bool passwordProtected);

    void handleStateChange(ServerCoordinator::State serverState);
    void refreshStatus();
};

void ServerStatusPrivate::initialize()
{
    // set up log listeners
    StatusHostname* hostnameLine = new StatusHostname;
    QObject::connect(hostnameLine, &GameEvent::activated, [hostnameLine, this]() {
        setHostname(hostnameLine->hostname());
    });
    coordinator->installGameEvent(hostnameLine);

    StatusPlayerNumbers* playerLine = new StatusPlayerNumbers;
    QObject::connect(playerLine, &GameEvent::activated, [playerLine, this]() {
        setPlayerCount(playerLine->playerCount());
        setMaxPlayers(playerLine->maxPlayers());
    });
    coordinator->installGameEvent(playerLine);

    StatusMap* mapLine = new StatusMap;
    QObject::connect(mapLine, &GameEvent::activated, [mapLine, this]() {
        setMap(mapLine->map());
    });
    coordinator->installGameEvent(mapLine);

    StatusIpAddress* ipLine = new StatusIpAddress;
    QObject::connect(ipLine, &GameEvent::activated, [ipLine, this]() {
        QUrl address;
        address.setScheme("steam");
        address.setHost(ipLine->ip());
        address.setPort(static_cast<int>(ipLine->port()));
        setAddress(address);
    });
    coordinator->installGameEvent(ipLine);

    PlayerConnected* playerConnected = new PlayerConnected;
    QObject::connect(playerConnected, &GameEvent::activated, [this]() {
        setPlayerCount(playerCount + 1);
    });
    coordinator->installGameEvent(playerConnected);

    PlayerDropped* playerDropped = new PlayerDropped;
    QObject::connect(playerDropped, &GameEvent::activated, [this]() {
        setPlayerCount(playerCount - 1);
    });
    coordinator->installGameEvent(playerDropped);

    CvarValue* password = new CvarValue("sv_password");
    QObject::connect(password, &GameEvent::activated, [password, this]() {
        setPasswordProtected(!password->value().isEmpty());
    });
    coordinator->installGameEvent(password);
}

void ServerStatusPrivate::reset()
{
    setHostname(QString());
    setPlayerCount(0);
    setMaxPlayers(0);
    setMap(QString());
    setAddress(QUrl());
    setPasswordProtected(false);
}

void ServerStatusPrivate::setHostname(const QString& hostname)
{
    this->hostname = hostname;
    emit q->hostnameChanged(hostname);
}

void ServerStatusPrivate::setPlayerCount(int playerCount)
{
    this->playerCount = playerCount;
    emit q->playerCountChanged(playerCount);
}

void ServerStatusPrivate::setMaxPlayers(int maxPlayers)
{
    this->maxPlayers = maxPlayers;
    emit q->maxPlayersChanged(maxPlayers);
}

void ServerStatusPrivate::setMap(const QString& map)
{
    this->map = map;
    emit q->mapChanged(map);
}

void ServerStatusPrivate::setAddress(const QUrl& address)
{
    this->address = address;
    emit q->addressChanged(address);
    emit q->addressChanged(address.toString());
}

void ServerStatusPrivate::setPasswordProtected(bool passwordProtected)
{
    this->passwordProtected = passwordProtected;
    emit q->passwordProtectedChanged(passwordProtected);
}

void ServerStatusPrivate::handleStateChange(ServerCoordinator::State serverState)
{
    switch (serverState) {
        case ServerCoordinator::State::Running:
            QTimer::singleShot(0, std::bind(&ServerStatusPrivate::refreshStatus, this));
            break;

        default:
            reset();
            break;
    }
}

void ServerStatusPrivate::refreshStatus()
{
    // FIXME Execute rcon command instead of this
    coordinator->sendCommand("status");
    coordinator->sendCommand("sv_password");
}

ServerStatus::ServerStatus(ServerCoordinator* coordinator, QObject* parent) :
    QObject(parent),
    d(new ServerStatusPrivate(this, coordinator))
{
    connect(coordinator, &ServerCoordinator::stateChanged, std::bind(&ServerStatusPrivate::handleStateChange, d.data(), std::placeholders::_1));
    d->initialize();

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

} // namespace morgoth
