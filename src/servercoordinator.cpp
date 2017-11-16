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
    connect(qApp, &QCoreApplication::aboutToQuit, this, &ServerCoordinator::stop);
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
    stop();
    m_logListener->wait();
    QCoreApplication::processEvents();
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

bool ServerCoordinator::start()
{
    if (!server()->isValid()) {
        qWarning("%s is not installed properly", qPrintable(server()->name()));
        return false;
    }

    qDebug("%s: starting...", qPrintable(server()->name()));

    if (!m_tmux.create()) {
        qWarning("%s: could not create a tmux session", qPrintable(server()->name()));
        return false;
    }

    m_logFileName = QString("/tmp/morgoth-%1").arg(m_tmux.name());
    int ret = mkfifo(qPrintable(m_logFileName), 0666);
    if (ret) {
        qWarning("%s: could not create fifo at %s", qPrintable(server()->name()), qPrintable(m_logFileName));
        return false;
    }

    m_logListener = new LogListener(m_logFileName, this);
    m_logListener->start();

    std::for_each(m_eventHandlers.begin(), m_eventHandlers.end(),
                  std::bind(&LogListener::installEventHandler, m_logListener, std::placeholders::_1));

    if (!m_tmux.redirectOutput(m_logFileName)) {
        qWarning("%s: could not redirect output to %s", qPrintable(server()->name()), qPrintable(m_logFileName));
        return false;
    }

    QString cmd = QString("%1/srcds_run +map cp_badlands").arg(server()->path());
    if (!m_tmux.sendKeys(cmd)) {
        qWarning("%s: could not start the server", qPrintable(server()->name()));
        return false;
    }

    return true;
}

void ServerCoordinator::stop()
{
    if (isStarted()) {
        qDebug("%s: stopping...", qPrintable(server()->name()));
        m_tmux.sendKeys("quit");
    }
}

bool ServerCoordinator::command(const QString& cmd)
{
    if (!isStarted()) {
        qWarning("ServerCoordinator::stop(): %s is not started", qPrintable(server()->name()));
        return false;
    }

    return m_tmux.sendKeys(cmd);
}

void ServerCoordinator::handleServerStarted()
{
    ServerStartedEvent* e = qobject_cast<ServerStartedEvent*>(sender());
    qDebug("%s: started %s", qPrintable(server()->name()), qPrintable(e->game()));
    m_started = true;
}

void ServerCoordinator::handleServerStopped()
{
    m_logListener->requestInterruption();
    m_started = false;
    unlink(qPrintable(m_logFileName));
    qDebug("%s: stopped", qPrintable(server()->name()));
}

} // namespace Morgoth
