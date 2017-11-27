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

#include "eventhandler.h"
#include "server.h"
#include "tmuxsessionwrapper.h"
#include <QtCore/QObject>

namespace morgoth {

class LogListener;

/**
 * \brief The ServerCoordinator class supervises the server running process.
 */
class ServerCoordinator : public QObject {
    Q_OBJECT

    /**
     * The server process status.
     */
    Q_PROPERTY(ServerCoordinator::Status status READ status NOTIFY statusChanged)

    /**
     * The server instance.
     */
    Q_PROPERTY(const Server* server READ server CONSTANT)

public:
    /**
     * \brief The Status enum describes the status of the server's process.
     */
    enum Status {
        Offline         /**< The server is offline. */,
        Starting        /**< The server is booting up. */,
        Running         /**< The server is running. */,
        ShuttingDown    /**< The 'quit' command was sent to the server, but the
                            process has not exited yet. */,
        Crashed         /**< The server has crashed. */
    };
    Q_ENUM(Status)

signals:
    void statusChanged(ServerCoordinator::Status status);

public:
    /**
     * \brief Creates a new \c ServerCoordinator instance that supervises the
     *  \c server process.
     * \param server The \c Server instance to operate on.
     */
    explicit ServerCoordinator(const Server* server);

    /**
     * \brief Stops the server immediately and destroys this
     *  \c ServerCoordinator instnace.
     */
    virtual ~ServerCoordinator();

    /**
     * \brief Attaches a new \c EventHandler to the log listener.
     */
    void installEventHandler(EventHandler* handler);

    /**
     * \brief Finds the \c EventHandler, using its name as a key.
     * \param name The event handler's name to find.
     * \return The \c EventHandler instance, \c nullptr if nothing was found.
     */
    EventHandler* findEvent(const QString& name);

    const Server* server() const { return m_server; }
    Status status() const { return m_status; }

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
    void setStatus(Status status);
    void handleServerStarted();
    void handleServerStopped();
    void stopSync();

private:
    const Server* m_server;
    Status m_status = Status::Offline;
    TmuxSessionWrapper m_tmux;
    QString m_logFileName;
    LogListener* m_logListener = nullptr;
    QMap<QString, EventHandler*> m_eventHandlers;

};

} // namespace Morgoth

#endif // SERVERCOORDINATOR_H
