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
#include "morgothdaemon.h"
#include "serverstatusadaptor.h"
#include <QtCore>
#include <functional>

namespace morgoth {

class ServerStatusPrivate {
    Q_DISABLE_COPY(ServerStatusPrivate)
    Q_DECLARE_PUBLIC(ServerStatus)
    ServerStatus *const q_ptr;

public:
    explicit ServerStatusPrivate(ServerStatus* status, Server* server);

    void trackGameServer(org::morgoth::connector::GameServer* gameServer);
    void handleStateChange(ServerCoordinator::State serverState);
    void handleConVarChange(QString conVarName, QString newValue);

    void reset();
    void setHostname(const QString& hostname);
    void setPlayerCount(int playerCount);
    void setMaxPlayers(int maxPlayers);
    void setMap(const QString& map);
    void setAddress(const QUrl& address);
    void setAddressFromString(const QString& addressString);
    void setPassword(const QString& password);
    void setStvPort(int stvPort);
    void setStvPortFromString(const QString& stvPortString);
    void setStvPassword(const QString& stvPassword);
    void addPlayer(const PlayerInfo& player);
    void removePlayer(int id);
    void clearPlayers();

    Server* server;
    QString hostname;
    int playerCount = 0;
    int maxPlayers = 0;
    QString map;
    QUrl address;
    QString password;
    int stvPort = 0;
    QString stvPassword;
    QList<PlayerInfo> players;
};

ServerStatusPrivate::ServerStatusPrivate(ServerStatus* status, Server* server) :
    q_ptr(status),
    server(server)
{
    QObject::connect(server->coordinator(), &ServerCoordinator::stateChanged, status,
                     [this](ServerCoordinator::State state) { handleStateChange(state); });
    QObject::connect(server, &Server::gameServerOnline, status,
                     [this](org::morgoth::connector::GameServer* gameServer) { trackGameServer(gameServer); });
}

void ServerStatusPrivate::trackGameServer(org::morgoth::connector::GameServer* gameServer)
{
    Q_Q(ServerStatus);
    using org::morgoth::connector::GameServer;

    setMaxPlayers(gameServer->maxPlayers());
    setAddressFromString(gameServer->address());

    QObject::connect(gameServer, &GameServer::mapChanged, q,
                     [this](const QString& map) { setMap(map); });
    setMap(gameServer->map());

    QObject::connect(gameServer, &GameServer::conVarChanged, q,
                     [this](const QString& conVarName, const QString& newValue) { handleConVarChange(conVarName, newValue); });

    gameServer->watchConVar("hostname");
    setHostname(gameServer->getConVarValue("hostname"));
    gameServer->watchConVar("sv_password");
    setPassword(gameServer->getConVarValue("sv_password"));

    // SourceTV details
    gameServer->watchConVar("tv_password");
    setStvPassword(gameServer->getConVarValue("tv_password"));
    gameServer->watchConVar("tv_port");
    setStvPortFromString(gameServer->getConVarValue("tv_port"));

    // player tracking
    QObject::connect(gameServer, &GameServer::playerConnected, q, [this, gameServer](int id) {
        PlayerInfo player(id);
        player.setName(gameServer->getPlayerName(id));
        player.setSteamId(SteamId(gameServer->getPlayerSteamId(id)));
        addPlayer(player);
    });
    QObject::connect(gameServer, &GameServer::playerDisconnected, q,
                     [this](int id) { removePlayer(id); });
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
    clearPlayers();
}

void ServerStatusPrivate::setHostname(const QString& hostname)
{
    Q_Q(ServerStatus);
    this->hostname = hostname;
    emit q->hostnameChanged(this->hostname);
}

void ServerStatusPrivate::setPlayerCount(int playerCount)
{
    Q_Q(ServerStatus);
    this->playerCount = playerCount;
    emit q->playerCountChanged(playerCount);
}

void ServerStatusPrivate::setMaxPlayers(int maxPlayers)
{
    Q_Q(ServerStatus);
    this->maxPlayers = maxPlayers;
    emit q->maxPlayersChanged(maxPlayers);
}

void ServerStatusPrivate::setMap(const QString& map)
{
    Q_Q(ServerStatus);
    this->map = map;
    emit q->mapChanged(this->map);
}

void ServerStatusPrivate::setAddress(const QUrl& address)
{
    Q_Q(ServerStatus);
    this->address = address;
    emit q->addressChanged(this->address);
    // for D-Bus compatibility
    emit q->addressChanged(address.toString());
}

void ServerStatusPrivate::setAddressFromString(const QString& addressString)
{
    QUrl address;
    address.setScheme("steam");
    auto s = addressString.split(':');
    Q_ASSERT(s.length() == 2);
    address.setHost(s.at(0));
    address.setPort(s.at(1).toInt());
    setAddress(address);
}

void ServerStatusPrivate::setPassword(const QString& password)
{
    Q_Q(ServerStatus);
    this->password = password;
    emit q->passwordChanged(this->password);
}

void ServerStatusPrivate::setStvPort(int stvPort)
{
    Q_Q(ServerStatus);
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
    Q_Q(ServerStatus);
    this->stvPassword = stvPassword;
    emit q->stvPasswordChanged(this->stvPassword);
}

void ServerStatusPrivate::addPlayer(const PlayerInfo& player)
{
    Q_Q(ServerStatus);
    players.append(player);
    setPlayerCount(playerCount + 1);
    emit q->playersChanged(players);
}

void ServerStatusPrivate::removePlayer(int id)
{
    Q_Q(ServerStatus);
    auto player = std::find_if(players.begin(), players.end(), [id](const auto& p) { return p.id() == id; });
    players.erase(player);
    setPlayerCount(playerCount - 1);
    emit q->playersChanged(players);
}

void ServerStatusPrivate::clearPlayers()
{
    Q_Q(ServerStatus);
    players.clear();
    setPlayerCount(0);
    emit q->playersChanged(players);
}


ServerStatus::ServerStatus(Server* server) :
    QObject(server),
    d_ptr(new ServerStatusPrivate(this, server))
{
    new ServerStatusAdaptor(this);
    if (morgothd)
        morgothd->dbusConnection().registerObject(server->statusPath().path(), this);
}

ServerStatus::~ServerStatus()
{

}

const QString& ServerStatus::hostname() const
{
    Q_D(const ServerStatus);
    return d->hostname;
}

int ServerStatus::playerCount() const
{
    Q_D(const ServerStatus);
    return d->playerCount;
}

int ServerStatus::maxPlayers() const
{
    Q_D(const ServerStatus);
    return d->maxPlayers;
}

QString ServerStatus::map() const
{
    Q_D(const ServerStatus);
    return d->map;
}

QUrl ServerStatus::address() const
{
    Q_D(const ServerStatus);
    return d->address;
}

QString ServerStatus::password() const
{
    Q_D(const ServerStatus);
    return d->password;
}

int ServerStatus::stvPort() const
{
    Q_D(const ServerStatus);
    return d->stvPort;
}

QString ServerStatus::stvPassword() const
{
    Q_D(const ServerStatus);
    return d->stvPassword;
}

const QList<PlayerInfo>& ServerStatus::players() const
{
    Q_D(const ServerStatus);
    return d->players;
}

} // namespace morgoth
