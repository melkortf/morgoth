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
#include <QtCore>

namespace {
class StatusPlayerLineEvent : public morgoth::EventHandler {
    Q_OBJECT

public:
    StatusPlayerLineEvent(QObject* parent = nullptr) : morgoth::EventHandler("status.playersinfo", parent) {}

    QRegularExpression regex() const override
    {
        return QRegularExpression("^players\\s+\\:\\s+(\\d+)\\shumans,\\s(\\d+)\\sbots\\s\\((\\d+)\\smax\\)$");
    }

    int players;
    int maxPlayers;

protected:
    void maybeActivated(const QString& line, const QRegularExpressionMatch& match) override
    {
        Q_UNUSED(line);
        players = match.captured(1).toInt();
        maxPlayers = match.captured(3).toInt();
        emit activated();
    }
};

class StatusMapLineEvent : public morgoth::EventHandler {
    Q_OBJECT

public:
    StatusMapLineEvent(QObject* parent = nullptr) : morgoth::EventHandler("status.mapinfo", parent) {}

    QRegularExpression regex() const override
    {
        return QRegularExpression("^map\\s+\\:\\s+(\\w+).*$");
    }

    QString map;

protected:
    void maybeActivated(const QString& line, const QRegularExpressionMatch& match) override
    {
        Q_UNUSED(line);
        map = match.captured(1);
        emit activated();
    }
};
}

namespace morgoth {

ServerStatus::ServerStatus(ServerCoordinator* coordinator, QObject* parent) :
    QObject(parent),
    m_coordinator(coordinator)
{
    StatusPlayerLineEvent* playerLine = new StatusPlayerLineEvent;
    connect(playerLine, &EventHandler::activated, [playerLine, this]() {
        setPlayerCount(playerLine->players);
        setMaxPlayers(playerLine->maxPlayers);
    });
    m_coordinator->installEventHandler(playerLine);

    StatusMapLineEvent* mapLine = new StatusMapLineEvent;
    connect(mapLine, &EventHandler::activated, [mapLine, this]() {
        setMap(mapLine->map);
    });
    m_coordinator->installEventHandler(mapLine);
}

void ServerStatus::reset()
{
    setPlayerCount(0);
    setMaxPlayers(0);
    setMap(QString());
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
    m_coordinator->sendCommand("status");
}

} // namespace morgoth

#include "serverstatus.moc"
