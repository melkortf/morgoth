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
#include "loglistener.h"
#include "mapchangeevent.h"
#include "serverstartedevent.h"
#include "serverstoppedevent.h"
#include <QtCore>
#include <functional>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace morgoth {

ServerCoordinator::ServerCoordinator(const Server* server) :
    m_server(server)
{
    connect(qApp, &QCoreApplication::aboutToQuit, this, &ServerCoordinator::stopSync);
    installEventHandler(new MapChangeEvent);

    ServerStartedEvent* serverStarted = new ServerStartedEvent;
    connect(serverStarted, &EventHandler::activated, this, &ServerCoordinator::handleServerStarted);
    installEventHandler(serverStarted);

    ServerStoppedEvent* serverStopped = new ServerStoppedEvent;
    connect(serverStopped, &EventHandler::activated, this, &ServerCoordinator::handleServerStopped);
    installEventHandler(serverStopped);
}

ServerCoordinator::~ServerCoordinator()
{
    stopSync();
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

void ServerCoordinator::setStatus(ServerCoordinator::Status status)
{
    m_status = status;
    emit statusChanged(m_status);
}

bool ServerCoordinator::start()
{
    if (!server()->isValid()) {
        qWarning("%s is not installed properly", qPrintable(server()->name()));
        return false;
    }

    if (status() != Offline) {
        qWarning("%s is already running", qPrintable(server()->name()));
        return false;
    }

    qInfo("%s: starting...", qPrintable(server()->name()));
    setStatus(Starting);

    if (!m_tmux.create()) {
        qWarning("%s: could not create a tmux session", qPrintable(server()->name()));
        setStatus(Crashed);
        return false;
    }

    m_logFileName = QString("/tmp/%1-tmux").arg(m_tmux.name());
    int ret = mkfifo(m_logFileName.toLocal8Bit().constData(), 0666);
    if (ret) {
        qWarning("%s: could not create fifo at %s", qPrintable(server()->name()), qPrintable(m_logFileName));
        m_tmux.kill();
        setStatus(Crashed);
        return false;
    }

    m_logListener = new LogListener(m_logFileName, this);
    m_logListener->start();

    std::for_each(m_eventHandlers.begin(), m_eventHandlers.end(),
                  std::bind(&LogListener::installEventHandler, m_logListener, std::placeholders::_1));

    if (!m_tmux.redirectOutput(m_logFileName)) {
        qWarning("%s: could not redirect output to %s", qPrintable(server()->name()), qPrintable(m_logFileName));
        m_tmux.kill();
        unlink(m_logFileName.toLocal8Bit().constData());
        setStatus(Crashed);
        return false;
    }

    QString arguments = server()->launchArguments().asSrcdsArguments();
    QString cmd = QString("%1/srcds_run %2").arg(server()->path().toLocalFile(), arguments);
    if (!m_tmux.sendKeys(cmd)) {
        qWarning("%s: could not start the server", qPrintable(server()->name()));
        m_tmux.kill();
        unlink(m_logFileName.toLocal8Bit().constData());
        setStatus(Crashed);
        return false;
    }

    return true;
}

void ServerCoordinator::stop()
{
    if (status() == Running || status() == Starting) {
        qDebug("%s: stopping...", qPrintable(server()->name()));
        setStatus(ShuttingDown);
        m_tmux.sendKeys("quit");
    }
}

void ServerCoordinator::handleServerStarted()
{
    ServerStartedEvent* e = qobject_cast<ServerStartedEvent*>(sender());
    qInfo("%s: started %s", qPrintable(server()->name()), qPrintable(e->game()));
    setStatus(Running);
}

void ServerCoordinator::handleServerStopped()
{
    Q_ASSERT(m_logListener);
    m_logListener->requestInterruption();
    m_logListener = nullptr;
    unlink(m_logFileName.toLocal8Bit().constData());
    m_tmux.kill();
    Q_ASSERT(status() == ShuttingDown);
    setStatus(Offline);
    qInfo("%s: stopped", qPrintable(server()->name()));
}

void ServerCoordinator::stopSync()
{
    // send stop keys and wait for the server to actually shut down
    stop();

    if (m_logListener) {
        m_logListener->wait();
    }

    m_tmux.kill();
    unlink(m_logFileName.toLocal8Bit().constData());
}

} // namespace Morgoth
