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
#include "playerinfo.h"
#include "servercoordinator.h"
#include <QtCore/QObject>

namespace morgoth {

class ServerStatusPrivate;

/**
 * \brief The ServerStatus class stores runtime server information.
 */
class MORGOTH_EXPORT ServerStatus : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.ServerStatus")

    /**
     * The server's hostname, as stored in the \c hostname cvar.
     */
    Q_PROPERTY(QString hostname READ hostname NOTIFY hostnameChanged)

    /**
     * Number of players currently connected to the server.
     */
    Q_PROPERTY(int playerCount READ playerCount NOTIFY playerCountChanged)

    /**
     * Maximum number of players.
     * Defined by the \c +maxplayers argument.
     */
    Q_PROPERTY(int maxPlayers READ maxPlayers NOTIFY maxPlayersChanged)

    /**
     * A map the server is currently running.
     */
    Q_PROPERTY(QString map READ map NOTIFY mapChanged)

    /**
     * Public IP address of the server.
     */
    Q_PROPERTY(QUrl address READ address NOTIFY addressChanged)

    /**
     * Holds the server's current server password (the sv_password cvar).
     */
    Q_PROPERTY(QString password READ password NOTIFY passwordChanged)

    /**
     * The SourceTV port for this server.
     */
    Q_PROPERTY(int stvPort READ stvPort NOTIFY stvPortChanged)

    /**
     * The SourceTV password for this server.
     */
    Q_PROPERTY(QString stvPassword READ stvPassword NOTIFY stvPasswordChanged)

    /**
      * List of players currently connected to this server.
      */
    Q_PROPERTY(morgoth::PlayerInfoList players READ players NOTIFY playersChanged)

signals:
    void hostnameChanged(const QString& hostname);
    void playerCountChanged(int playerCount);
    void maxPlayersChanged(int maxPlayers);
    void mapChanged(const QString& map);
    void addressChanged(const QUrl& address);
    void passwordChanged(const QString& password);
    void stvPortChanged(int stvPort);
    void stvPasswordChanged(const QString& stvPassword);
    void playersChanged(const PlayerInfoList& players);

    // DBus workaround to make setAutoRelaySignals() in adaptor work
    // (the D-Bus XML type is registered as a string).
    void addressChanged(const QString& address);

public:
    explicit ServerStatus(ServerCoordinator* coordinator, QObject *parent = nullptr);
    virtual ~ServerStatus();

    const QString& hostname() const;
    int playerCount() const;
    int maxPlayers() const;
    QString map() const;
    QUrl address() const;
    QString password() const;
    int stvPort() const;
    QString stvPassword() const;
    const QList<PlayerInfo>& players() const;

private:
    QScopedPointer<ServerStatusPrivate> d;

};

} // namespace morgoth

#endif // SERVERSTATUS_H
