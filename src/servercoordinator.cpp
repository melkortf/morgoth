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

#include "servercoordinator.h"
#include "logcollector.h"
#include "loglistener.h"
#include "morgothdaemon.h"
#include "serverconfiguration.h"
#include "servercoordinatoradaptor.h"
#include "gameevents/serverstarted.h"
#include "gameevents/serverstopped.h"
#include <QtCore>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

namespace morgoth {

ServerCoordinator::ServerCoordinator(Server* server) :
    QObject(server),
    m_server(server)
{
    Q_ASSERT(server->isValid());

    QString logDirectory = server->configuration()->value("org.morgoth.Server.logDirectory");
    if (QDir::isRelativePath(logDirectory))
        logDirectory.prepend(server->path().toLocalFile() + QDir::separator());

    m_logCollector = new LogCollector(QDir::cleanPath(logDirectory), this);
    connect(this, &ServerCoordinator::stateChanged, m_logCollector, &LogCollector::save);

    connect(qApp, &QCoreApplication::aboutToQuit, this, &ServerCoordinator::stopSync);

    ServerStarted* serverStarted = new ServerStarted;
    connect(serverStarted, &EventHandler::activated, this, &ServerCoordinator::handleServerStarted);
    installEventHandler(serverStarted);

    ServerStopped* serverStopped = new ServerStopped;
    connect(serverStopped, &EventHandler::activated, this, &ServerCoordinator::handleServerStopped);
    installEventHandler(serverStopped);

    new ServerCoordinatorAdaptor(this);

    if (morgothd)
        morgothd->dbusConnection().registerObject(server->coordinatorPath().path(), this);
}

ServerCoordinator::~ServerCoordinator()
{
    // stopSync() cannot be used here, as the parent Server instance
    // is already destroyed and we relay on its existence
}

void ServerCoordinator::installEventHandler(EventHandler* handler)
{
    m_eventHandlers.insert(handler->name(), handler);
    if (m_logListener)
        m_logListener->installEventHandler(handler);
}

EventHandler* ServerCoordinator::findEvent(const QString& name)
{
    return m_eventHandlers.value(name, nullptr);
}

void ServerCoordinator::sendCommand(const QString& command)
{
    if (state() == Running)
        m_tmux.sendKeys(command);
}

void ServerCoordinator::setState(ServerCoordinator::State state)
{
    m_state = state;
    emit stateChanged(m_state);
}

bool ServerCoordinator::start()
{
    Q_ASSERT(server()->configuration());

    if (!server()->isValid()) {
        qWarning("%s is not installed properly", qPrintable(server()->name()));
        return false;
    }

    if (state() != Offline) {
        qWarning("%s is already running", qPrintable(server()->name()));
        return false;
    }

    qInfo("%s: starting...", qPrintable(server()->name()));
    setState(Starting);

    QString user = server()->configuration()->value("org.morgoth.Server.user");
    if (user.isEmpty() && morgothd)
        user = morgothd->config().value("user").toString();

    m_tmux.setUser(user);

    if (!m_tmux.create()) {
        qWarning("%s: could not create a tmux session", qPrintable(server()->name()));
        setState(Crashed);
        return false;
    }

    m_outputFileName = QString("/tmp/%1-tmux").arg(m_tmux.name());
    if (!createFifo(m_outputFileName, user)) {
        m_tmux.kill();
        setState(Crashed);
        return false;
    }

    m_logListener = new LogListener(m_outputFileName, this);
    m_logListener->setLogCollector(m_logCollector);
    m_logListener->start();

    std::for_each(m_eventHandlers.begin(), m_eventHandlers.end(),
                  std::bind(&LogListener::installEventHandler, m_logListener, std::placeholders::_1));

    if (!m_tmux.redirectOutput(m_outputFileName)) {
        qWarning("%s: could not redirect output to %s", qPrintable(server()->name()), qPrintable(m_outputFileName));
        m_tmux.kill();
        unlink(m_outputFileName.toLocal8Bit().constData());
        setState(Crashed);
        return false;
    }

    QString arguments = server()->configuration()->value("org.morgoth.Server.launchArguments");
    QString cmd = QString("%1/srcds_run %2").arg(server()->path().toLocalFile(), arguments);
    if (!m_tmux.sendKeys(cmd)) {
        qWarning("%s: could not start the server", qPrintable(server()->name()));
        m_tmux.kill();
        unlink(m_outputFileName.toLocal8Bit().constData());
        setState(Crashed);
        return false;
    }

    return true;
}

void ServerCoordinator::stop()
{
    if (state() == Running || state() == Starting) {
        qDebug("%s: stopping...", qPrintable(server()->name()));
        setState(ShuttingDown);
        m_tmux.sendKeys("quit");
    }
}

void ServerCoordinator::flushLogs()
{
    m_logCollector->save();
}

bool ServerCoordinator::createFifo(const QString& fileName, const QString& owner)
{
    int ret = ::mkfifo(fileName.toLocal8Bit().constData(), 0666);
    if (ret) {
        qWarning("%s: could not create fifo at %s", qPrintable(server()->name()), qPrintable(fileName));
        return false;
    }

    if (!owner.isEmpty()) {
        passwd* pwd = ::getpwnam(owner.toLocal8Bit().constData());
        if (!pwd) {
            char* error = ::strerror(errno);
            qWarning("Error retrieving uid and gid of user %s (%s)", qPrintable(owner), error);
            unlink(fileName.toLocal8Bit().constData());
            return false;
        } else {
            ::chown(m_outputFileName.toLocal8Bit().constData(), pwd->pw_uid, pwd->pw_gid);
        }
    }

    return true;
}

void ServerCoordinator::handleServerStarted()
{
    ServerStarted* e = qobject_cast<ServerStarted*>(sender());
    qInfo("%s: started %s", qPrintable(server()->name()), qPrintable(e->game()));
    setState(Running);
}

void ServerCoordinator::handleServerStopped()
{
    Q_ASSERT(m_logListener);
    m_logListener->requestInterruption();
    m_logListener = nullptr;
    unlink(m_outputFileName.toLocal8Bit().constData());
    m_tmux.kill();
    Q_ASSERT(state() == ShuttingDown);
    setState(Offline);
    qInfo("%s: stopped", qPrintable(server()->name()));
}

void ServerCoordinator::stopSync()
{
    // send stop keys and wait for the server to actually shut down
    stop();

    if (m_logListener) {
        m_logListener->wait();
        m_logListener = nullptr;
    }

    m_tmux.kill();
    unlink(m_outputFileName.toLocal8Bit().constData());
}

} // namespace Morgoth

QDBusArgument& operator<<(QDBusArgument& argument, const morgoth::ServerCoordinator::State& state)
{
    QString strStatus = QMetaEnum::fromType<morgoth::ServerCoordinator::State>().valueToKey(state);

    argument.beginStructure();
    argument << strStatus;
    argument.endStructure();

    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, morgoth::ServerCoordinator::State& state)
{
    QString strStatus;

    argument.beginStructure();
    argument >> strStatus;
    argument.endStructure();

    int value = QMetaEnum::fromType<morgoth::ServerCoordinator::State>().keyToValue(strStatus.toLocal8Bit().constData());
    state = static_cast<morgoth::ServerCoordinator::State>(value);
    return argument;
}

static void registerMetaType()
{
    qDBusRegisterMetaType<morgoth::ServerCoordinator::State>();
}
Q_COREAPP_STARTUP_FUNCTION(registerMetaType)
