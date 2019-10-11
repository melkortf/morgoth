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

#ifndef SERVER_H
#define SERVER_H

#include "morgoth_export.h"
#include "gameserverinterface.h"
#include "serverconfiguration.h"
#include <QtCore/QObject>
#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtDBus/QDBusObjectPath>

namespace morgoth {
class ServerPrivate;
class ServerCoordinator;
class ServerStatus;

/**
 * \brief The Server class represents a single server installation.
 *
 * The \c Server instance contains all static information about the server.
 * A single server is identified by its \ref name. Its installation \ref path
 * contains the \c srcds_run script. To start or stop the server, execute
 * any command or access its status, use \ref ServerCoordinator.
 *
 * \sa ServerCoordinator and ServerManager.
 * \todo Handle remote servers.
 */
class MORGOTH_EXPORT Server : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.Server")

    /**
     * Name of this server.
     */
    Q_PROPERTY(QString name READ name CONSTANT)

    /**
     * Path to this server's installation.
     */
    Q_PROPERTY(QUrl path READ path CONSTANT)

    /**
     * Specifies whether this server is valid.
     */
    Q_PROPERTY(bool valid READ isValid CONSTANT)

    /**
     * This server's coordinator instance.
     */
    Q_PROPERTY(ServerCoordinator* coordinator READ coordinator CONSTANT)

    /**
     * This server's coordinator D-Bus path.
     */
    Q_PROPERTY(QDBusObjectPath coordinatorPath READ coordinatorPath CONSTANT)

    /**
     * This server's configuration.
     */
    Q_PROPERTY(ServerConfiguration* configuration READ configuration CONSTANT)

    /**
     * This server's configuration D-Bus path.
     */
    Q_PROPERTY(QDBusObjectPath configurationPath READ configurationPath CONSTANT)

    /**
     * This server's status.
     */
    Q_PROPERTY(ServerStatus* status READ status CONSTANT)

    /**
     * This server's status object D-Bus path.
     */
    Q_PROPERTY(QDBusObjectPath statusPath READ statusPath CONSTANT)

signals:
    /**
     * A game server was registerd on the D-Bus.
     */
    void gameServerOnline(org::morgoth::connector::GameServer* gameServer);

    /**
     * A game server has lost its connection with the D-Bus server.
     */
    void gameServerTimedOut();

public:
    /**
     * \brief Creates the new \c Server instance.
     * \param path Path to the directory where the server is installed.
     * \param name Name of the server.
     * \param parent Passed to QObject.
     * \sa ServerManager::add().
     */
    Server(const QUrl& path, const QString& name, QObject* parent = nullptr);

    /**
     * \brief Deletes this \c Server instance.
     */
    virtual ~Server();

    /**
     * \brief Returns path to the srcds_run script.
     * \note This function returns an empty string if \ref isValid() is \c false.
     */
    const QString& srcdsExec() const;

    const QString& name() const;
    const QUrl& path() const;
    bool isValid() const;
    ServerCoordinator* coordinator();
    const ServerCoordinator* coordinator() const;
    const QDBusObjectPath& coordinatorPath();
    ServerConfiguration* configuration();
    const ServerConfiguration* configuration() const;
    const QDBusObjectPath& configurationPath() const;
    ServerStatus* status();
    const ServerStatus* status() const;
    const QDBusObjectPath& statusPath() const;

private:
    Q_DECLARE_PRIVATE(Server)
    QScopedPointer<ServerPrivate> const d_ptr;

};

} // namespace Morgoth

Q_DECLARE_METATYPE(org::morgoth::connector::GameServer*)

#endif // SERVER_H
