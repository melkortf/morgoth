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

#ifndef SERVERSTATUS_H
#define SERVERSTATUS_H

#include "morgoth_export.h"
#include "servercoordinator.h"
#include <QtCore/QObject>

namespace morgoth {

class ServerStatus : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.ServerStatus")

    Q_PROPERTY(QString hostname READ hostname NOTIFY hostnameChanged)
    Q_PROPERTY(int playerCount READ playerCount NOTIFY playerCountChanged)
    Q_PROPERTY(int maxPlayers READ maxPlayers NOTIFY maxPlayersChanged)
    Q_PROPERTY(QString map READ map NOTIFY mapChanged)

signals:
    void hostnameChanged(const QString& hostname);
    void playerCountChanged(int playerCount);
    void maxPlayersChanged(int maxPlayers);
    void mapChanged(const QString& map);

public:
    explicit ServerStatus(ServerCoordinator* coordinator, QObject *parent = nullptr);

    const QString& hostname() const { return m_hostname; }
    int playerCount() const { return m_playerCount; }
    int maxPlayers() const { return m_maxPlayers; }
    QString map() const { return m_map; }

private:
    void reset();
    void setPlayerCount(int playerCount);
    void setMaxPlayers(int maxPlayers);
    void setMap(const QString& map);

private slots:
    void handleStateChange(ServerCoordinator::State serverState);
    void refreshStatus();

private:
    ServerCoordinator* m_coordinator;

    QString m_hostname;
    int m_playerCount = 0;
    int m_maxPlayers = 0;
    QString m_map;

};

} // namespace morgoth

#endif // SERVERSTATUS_H
