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

#include "gameserverinterface.h"
#include "morgothdaemon.h"
#include "servermanager.h"
#include "serverstarter.h"
#include <QtCore/QDir>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

using org::morgoth::connector::GameServer;

// The time we wait for the game server to become online after starting it
// 30 seconds
static constexpr auto GameServerStartTimeout = 30 * 1000;

namespace morgoth {

ServerStarter::ServerStarter(Server* server, TmuxSessionWrapper* tmuxSession) :
    QObject(server),
    m_server(server),
    m_tmuxSession(tmuxSession)
{
    Q_ASSERT(m_server);
    Q_ASSERT(m_tmuxSession);

    m_startTimeoutTimer.setSingleShot(true);
    m_startTimeoutTimer.setInterval(GameServerStartTimeout);
    connect(&m_startTimeoutTimer, &QTimer::timeout, this, &ServerStarter::timeoutStart);
}

ServerStarter::~ServerStarter()
{

}

void ServerStarter::start()
{
    emit starting();

    if (!m_server->isValid()) {
        emit failed(ServerError::InvalidInstallation);
        return;
    }

    if (!m_server->configuration()) {
        emit failed(ServerError::InvalidConfiguration);
        return;
    }

    QString user = m_server->configuration()->value("org.morgoth.Server.user");
    if (user.isEmpty() && morgothd)
        user = morgothd->config().value("user").toString();

    m_tmuxSession->setUser(user);

    if (!m_tmuxSession->create()) {
        emit failed(ServerError::UnableToCreateTmuxSession);
        return;
    }

    QString logDirectory = m_server->configuration()->value("org.morgoth.Server.logDirectory");
    if (QDir::isRelativePath(logDirectory))
        logDirectory.prepend(m_server->path().toLocalFile());

    QString logFileName = QString("%1/gameserver.log").arg(logDirectory);

    if (!user.isEmpty()) {
        passwd* pwd = ::getpwnam(user.toLocal8Bit().constData());
        if (!pwd) {
            char* error = ::strerror(errno);
            qWarning("%s", error);
            emit failed(ServerError::UnableToResolveUser);
            return;
        } else {
            ::chown(logFileName.toLocal8Bit().constData(), pwd->pw_uid, pwd->pw_gid);
        }
    }

    if (!m_tmuxSession->redirectOutput(logFileName)) {
        m_tmuxSession->kill();
        emit failed(ServerError::UnableToRedirectTmuxOutput);
        return;
    }

    QString arguments = m_server->configuration()->value("org.morgoth.Server.launchArguments");
    QString cmd = QString("%1/srcds_run %2").arg(m_server->path().toLocalFile(), arguments);
    if (!m_tmuxSession->sendKeys(cmd)) {
        m_tmuxSession->kill();
        emit failed(ServerError::ServerExecutableFailed);
        return;
    }

    connect(m_server, &Server::gameServerOnline, this, &ServerStarter::handleGameServerRegistered);
    m_startTimeoutTimer.start();
}

void ServerStarter::timeoutStart()
{
    emit failed(ServerError::ServerNotRegisteredOnTime);
}

void ServerStarter::handleGameServerRegistered(GameServer* gameServer)
{
    Q_ASSERT(QDir(m_server->path().toLocalFile()) == gameServer->gameLocation());
    m_startTimeoutTimer.stop();
    emit started(gameServer);
}

} // namespace Morgoth
