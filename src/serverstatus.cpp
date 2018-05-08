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
#include "gameevents/playerconnected.h"
#include "gameevents/playerdropped.h"
#include "gameevents/statushostname.h"
#include "gameevents/statusipaddress.h"
#include "gameevents/statusmap.h"
#include "gameevents/statusplayernumbers.h"
#include <QtCore>

namespace morgoth {

ServerStatus::ServerStatus(ServerCoordinator* coordinator, QObject* parent) :
    QObject(parent),
    m_coordinator(coordinator)
{
    connect(coordinator, &ServerCoordinator::stateChanged, this, &ServerStatus::handleStateChange);

    initialize();

    new ServerStatusAdaptor(this);
    if (morgothd)
        morgothd->dbusConnection().registerObject(coordinator->server()->statusPath().path(), this);
}

void ServerStatus::initialize()
{
    // set up log listeners
    StatusHostname* hostnameLine = new StatusHostname;
    connect(hostnameLine, &EventHandler::activated, [hostnameLine, this]() {
        setHostname(hostnameLine->hostname());
    });
    m_coordinator->installEventHandler(hostnameLine);

    StatusPlayerNumbers* playerLine = new StatusPlayerNumbers;
    connect(playerLine, &EventHandler::activated, [playerLine, this]() {
        setPlayerCount(playerLine->playerCount());
        setMaxPlayers(playerLine->maxPlayers());
    });
    m_coordinator->installEventHandler(playerLine);

    StatusMap* mapLine = new StatusMap;
    connect(mapLine, &EventHandler::activated, [mapLine, this]() {
        setMap(mapLine->map());
    });
    m_coordinator->installEventHandler(mapLine);

    StatusIpAddress* ipLine = new StatusIpAddress;
    connect(ipLine, &EventHandler::activated, [ipLine, this]() {
        QUrl address;
        address.setScheme("steam");
        address.setHost(ipLine->ip());
        address.setPort(static_cast<int>(ipLine->port()));
        setAddress(address);
    });
    m_coordinator->installEventHandler(ipLine);

    PlayerConnected* playerConnected = new PlayerConnected;
    connect(playerConnected, &EventHandler::activated, [this]() {
        setPlayerCount(playerCount() + 1);
    });
    m_coordinator->installEventHandler(playerConnected);

    PlayerDropped* playerDropped = new PlayerDropped;
    connect(playerDropped, &EventHandler::activated, [this]() {
        setPlayerCount(playerCount() - 1);
    });
    m_coordinator->installEventHandler(playerDropped);
}

void ServerStatus::reset()
{
    setHostname(QString());
    setPlayerCount(0);
    setMaxPlayers(0);
    setMap(QString());
    setAddress(QUrl());
}

void ServerStatus::setHostname(const QString& hostname)
{
    m_hostname = hostname;
    emit hostnameChanged(m_hostname);
}

void ServerStatus::setPlayerCount(int playerCount)
{
    m_playerCount = playerCount;
    emit playerCountChanged(m_playerCount);
}

void ServerStatus::setMaxPlayers(int maxPlayers)
{
    m_maxPlayers = maxPlayers;
    emit maxPlayersChanged(m_maxPlayers);
}

void ServerStatus::setMap(const QString& map)
{
    m_map = map;
    emit mapChanged(m_map);
}

void ServerStatus::setAddress(const QUrl& address)
{
    m_address = address;
    emit addressChanged(m_address);
    emit addressChanged(m_address.toString());
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
    m_coordinator->sendCommand("status");
}

} // namespace morgoth
