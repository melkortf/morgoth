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

#ifndef SERVERCOORDINATOR_H
#define SERVERCOORDINATOR_H

#include "morgoth_export.h"
#include "server.h"
#include "tmuxsessionwrapper.h"
#include <QtCore/QObject>
#include <QtDBus/QDBusArgument>

namespace morgoth {

class ServerCoordinatorPrivate;

/**
 * \brief The ServerCoordinator class supervises the server running process.
 */
class MORGOTH_EXPORT ServerCoordinator : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.ServerCoordinator")

    /**
     * The server process status.
     */
    Q_PROPERTY(morgoth::ServerCoordinator::State state READ state NOTIFY stateChanged)

    /**
     * The server instance.
     */
    Q_PROPERTY(const Server* server READ server CONSTANT)

public:
    /**
     * \brief The State enum describes the state of the server's process.
     */
    enum State {
        Offline         /**< The server is offline. */,
        Starting        /**< The server is booting up. */,
        Running         /**< The server is running. */,
        ShuttingDown    /**< The 'quit' command was sent to the server, but the
                            process has not exited yet. */,
        Crashed         /**< The server has crashed. */
    };
    Q_ENUM(State)

signals:
    void stateChanged(morgoth::ServerCoordinator::State state);

public:
    /**
     * \brief Creates a new \c ServerCoordinator instance that supervises the
     *  \c server process.
     * \param server The \c Server instance to operate on.
     */
    explicit ServerCoordinator(Server* server);

    /**
     * \brief Stops the server immediately and destroys this
     *  \c ServerCoordinator instnace.
     */
    virtual ~ServerCoordinator();

    /**
     * \brief Sends the given \c command to the server.
     * \note This method does nothing if the server is not running.
     * \param command The command to be sent.
     */
    void sendCommand(const QString& command);

    const Server* server() const;
    State state() const;

public slots:
    /**
     * \brief Starts the server.
     *
     * Calling this method spawns a new tmux session, pipes its output to
     * the log listener and then issues the server start command.
     * The server status will be changed to \c Starting and then to \c Running
     * once the server is fully up and running.
     *
     * \note This method does all the system calls synchronously, but the
     *  server itself starts detached.
     *
     * \return \c true if the server bootup process was successful; \c false
     *  otherwise.
     * \sa statusChanged() and stop().
     */
    bool start();

    /**
     * \brief Sends the "quit" command to the server.
     */
    void stop();

private slots:
    void setState(State state);
    void handleServerStarted();
    void handleServerStopped();
    void stopSync();

private:
    QScopedPointer<ServerCoordinatorPrivate> d;

};

} // namespace Morgoth

MORGOTH_EXPORT QDBusArgument& operator<<(QDBusArgument& argument, const morgoth::ServerCoordinator::State& state);
MORGOTH_EXPORT const QDBusArgument& operator>>(const QDBusArgument& argument, morgoth::ServerCoordinator::State& state);

#endif // SERVERCOORDINATOR_H
