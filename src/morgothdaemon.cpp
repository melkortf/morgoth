#include "morgothdaemon.h"
#include <QtCore>
#include <csignal>
#include <unistd.h>
#include <sys/socket.h>
#include <systemd/sd-daemon.h>

namespace {

int signalFd[2];

void signalHandler(int signal)
{
    ::write(signalFd[0], &signal, sizeof(signal));
}

// install the function above
void setupSignalHandlers()
{
    struct sigaction hup, term;

    if (::socketpair(AF_UNIX, SOCK_STREAM, 0, signalFd))
        qFatal("socketpair failed");

    hup.sa_handler = signalHandler;
    sigemptyset(&hup.sa_mask);
    hup.sa_flags = 0;
    hup.sa_flags |= SA_RESTART;

    if (sigaction(SIGHUP, &hup, nullptr))
        qFatal("sigaction failed");

    term.sa_handler = signalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags |= SA_RESTART;

    if (sigaction(SIGTERM, &term, nullptr))
       qFatal("sigaction failed");
}

// https://www.freedesktop.org/software/systemd/man/sd-daemon.html#
void systemdMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(context);

    QString prefix;
    // enum QtMsgType { QtDebugMsg, QtWarningMsg, QtCriticalMsg, QtFatalMsg, QtInfoMsg, QtSystemMsg = QtCriticalMsg };
    switch (type) {
        case QtFatalMsg:
            prefix = SD_EMERG;
            break;
        case QtCriticalMsg:
            prefix = SD_CRIT;
            break;
        case QtWarningMsg:
            prefix = SD_WARNING;
            break;
        case QtInfoMsg:
            prefix = SD_INFO;
            break;
        case QtDebugMsg:
            prefix = SD_DEBUG;
            break;
    }

    fprintf(stderr, "%s%s\n", qPrintable(prefix), qPrintable(msg));
}

void installMessageHandler()
{
    qInstallMessageHandler(systemdMessageHandler);
}

}

namespace morgoth {

MorgothDaemon::MorgothDaemon(QObject* parent) :
    QObject(parent)
{
    m_signal = new QSocketNotifier(signalFd[1], QSocketNotifier::Read, this);
    connect(m_signal, &QSocketNotifier::activated, this, &MorgothDaemon::handleSignal);
}

void MorgothDaemon::handleSignal()
{
    m_signal->setEnabled(false);
    int signal;
    ::read(signalFd[1], &signal, sizeof(signal));

    switch (signal) {
        case SIGTERM:
            qInfo("-- SIGTERM --");
            QCoreApplication::quit();
            break;

        case SIGHUP:
            qInfo("-- SIGHUP --");
            // do something
            break;

        default:
            qInfo("Caught unhandled signal (%d)", signal);
    }

    m_signal->setEnabled(true);
}

} // namespace morgoth

Q_COREAPP_STARTUP_FUNCTION(setupSignalHandlers)
Q_COREAPP_STARTUP_FUNCTION(installMessageHandler)
