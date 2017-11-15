#include "servercoordinator.h"
#include "loglistener.h"
#include <QtCore>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

namespace Morgoth {

ServerCoordinator::ServerCoordinator(const Server *server) :
    m_server(server)
{
    connect(qApp, &QCoreApplication::aboutToQuit, this, &ServerCoordinator::stop);
}

ServerCoordinator::~ServerCoordinator()
{
    stop();
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

    m_logListener->addEvent(QRegularExpression(".*Host_Changelevel.*"), [this](auto, auto) {
        qDebug("%s: map change", qPrintable(this->server()->name()));
    });

    if (!m_tmux.redirectOutput(m_logFileName)) {
        qWarning("%s: could not redirect output to %s", qPrintable(server()->name()), qPrintable(m_logFileName));
        return false;
    }

    QString cmd = QString("%1/srcds_run +map cp_badlands").arg(server()->path());
    if (!m_tmux.sendKeys(cmd)) {
        qWarning("%s: could not start the server", qPrintable(server()->name()));
        return false;
    }

    m_started = true;
    qDebug("%s: started", qPrintable(server()->name()));

    return m_started;
}

void ServerCoordinator::stop()
{
    if (isStarted()) {
        qDebug("%s: stopping...", qPrintable(server()->name()));
        m_tmux.sendKeys("quit");

        unlink(qPrintable(m_logFileName));
        m_logListener->requestInterruption();
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

} // namespace Morgoth
