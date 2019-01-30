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
    QString password;
    int stvPort;
    QString stvPassword;
    QList<PlayerInfo> players;

    void reset();
    void setHostname(const QString& hostname);
    void setPlayerCount(int playerCount);
    void setMaxPlayers(int maxPlayers);
    void setMap(const QString& map);
    void setAddress(const QUrl& address);
    void setPassword(const QString& password);
    void setStvPort(int stvPort);
    void setStvPortFromString(const QString& stvPortString);
    void setStvPassword(const QString& stvPassword);
    void addPlayer(const PlayerInfo& player);
    void removePlayer(const PlayerInfo& player);

    void handleStateChange(ServerCoordinator::State serverState);
    void handleConVarChange(QString conVarName, QString newValue);
};

void ServerStatusPrivate::reset()
{
    setHostname(QString());
    setPlayerCount(0);
    setMaxPlayers(0);
    setMap(QString());
    setAddress(QUrl());
    setPassword(QString());
    setStvPort(0);
    setStvPassword(QString());
}

void ServerStatusPrivate::setHostname(const QString& hostname)
{
    this->hostname = hostname;
    emit q->hostnameChanged(this->hostname);
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
    emit q->mapChanged(this->map);
}

void ServerStatusPrivate::setAddress(const QUrl& address)
{
    this->address = address;
    emit q->addressChanged(this->address);
    emit q->addressChanged(address.toString());
}

void ServerStatusPrivate::setPassword(const QString& password)
{
    this->password = password;
    emit q->passwordChanged(this->password);
}

void ServerStatusPrivate::setStvPort(int stvPort)
{
    this->stvPort = stvPort;
    emit q->stvPortChanged(stvPort);
}

void ServerStatusPrivate::setStvPortFromString(const QString& stvPortString)
{
    bool ok;
    int stvPort = stvPortString.toInt(&ok);
    if (ok) {
        setStvPort(stvPort);
    } else {
        qWarning("%s is an invalid STV port value", qPrintable(stvPortString));
    }
}

void ServerStatusPrivate::setStvPassword(const QString& stvPassword)
{
    this->stvPassword = stvPassword;
    emit q->stvPasswordChanged(this->stvPassword);
}

void ServerStatusPrivate::addPlayer(const PlayerInfo& player)
{
    players.append(player);
    setPlayerCount(playerCount + 1);
    emit q->playersChanged(players);
}

void ServerStatusPrivate::removePlayer(const PlayerInfo& player)
{
    players.removeAll(player);
    setPlayerCount(playerCount - 1);
    emit q->playersChanged(players);
}

void ServerStatusPrivate::handleStateChange(ServerCoordinator::State serverState)
{
    switch (serverState) {
        case ServerCoordinator::State::Offline:
            reset();
            break;

        default:
            break;
    }
}

void ServerStatusPrivate::handleConVarChange(QString conVarName, QString newValue)
{
    if (conVarName == "hostname") {
        setHostname(newValue);
    } else if (conVarName == "sv_password") {
        setPassword(newValue);
    } else if (conVarName == "tv_port") {
        setStvPortFromString(newValue);
    } else if (conVarName == "tv_password") {
        setStvPassword(newValue);
    }
}

ServerStatus::ServerStatus(ServerCoordinator* coordinator, QObject* parent) :
    QObject(parent),
    d(new ServerStatusPrivate(this, coordinator))
{
    connect(coordinator, &ServerCoordinator::stateChanged, std::bind(&ServerStatusPrivate::handleStateChange, d.data(), std::placeholders::_1));

    new ServerStatusAdaptor(this);
    if (morgothd)
        morgothd->dbusConnection().registerObject(coordinator->server()->statusPath().path(), this);
}

ServerStatus::~ServerStatus()
{

}

void ServerStatus::trackGameServer(org::morgoth::connector::GameServer* gameServer)
{
    using org::morgoth::connector::GameServer;

    d->setMaxPlayers(gameServer->maxPlayers());

    connect(gameServer, &GameServer::mapChanged, std::bind(&ServerStatusPrivate::setMap, d.data(), std::placeholders::_1));
    d->setMap(gameServer->map());

    auto setAddressFromString = [this](const QString& str) {
        QUrl address;
        address.setScheme("steam");
        auto s = str.split(':');
        Q_ASSERT(s.length() == 2);
        address.setHost(s.at(0));
        address.setPort(s.at(1).toInt());
        d->setAddress(address);
    };

    setAddressFromString(gameServer->address());

    connect(gameServer, &GameServer::conVarChanged, std::bind(&ServerStatusPrivate::handleConVarChange, d.data(), std::placeholders::_1, std::placeholders::_2));
    gameServer->watchConVar("hostname");
    d->setHostname(gameServer->getConVarValue("hostname"));
    gameServer->watchConVar("sv_password");
    d->setPassword(gameServer->getConVarValue("sv_password"));

    // SourceTV details
    gameServer->watchConVar("tv_password");
    d->setStvPassword(gameServer->getConVarValue("tv_password"));
    gameServer->watchConVar("tv_port");
    d->setStvPortFromString(gameServer->getConVarValue("tv_port"));
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

QString ServerStatus::password() const
{
    return d->password;
}

int ServerStatus::stvPort() const
{
    return d->stvPort;
}

QString ServerStatus::stvPassword() const
{
    return d->stvPassword;
}

const QList<PlayerInfo>& ServerStatus::players() const
{
    return d->players;
}

} // namespace morgoth
