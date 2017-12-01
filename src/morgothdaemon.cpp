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

#include "morgothdaemon.h"
#include "configuration.h"
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

MorgothDaemon::MorgothDaemon(int& argc, char** argv) :
    QCoreApplication(argc, argv),
    m_dbusConnection(QDBusConnection::sessionBus())
{
    ::installMessageHandler();
    ::setupSignalHandlers();

    m_signal = new QSocketNotifier(signalFd[1], QSocketNotifier::Read, this);
    connect(m_signal, &QSocketNotifier::activated, this, &MorgothDaemon::handleSignal);

    parseArguments();

    // switch to system bus if the config says so
    QString dbus = configuration()->value("dbus", "session").toString();
    if (dbus == QStringLiteral("system"))
        m_dbusConnection = QDBusConnection::systemBus();

    // TODO reflect changes in config
}

void MorgothDaemon::parseArguments()
{
    QCommandLineParser parser;
    parser.addVersionOption();

    QCommandLineOption configFileOption(QStringList() << "c" << "config", "Config file to use", "config");
    parser.addOption(configFileOption);

    parser.process(*this);

    QString configFile = parser.isSet(configFileOption) ? parser.value(configFileOption) : QString();
    m_configuration = new Configuration(configFile);
    setProperty("configuration", QVariant::fromValue(m_configuration));
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

        case SIGHUP: {
            qInfo("-- SIGHUP --");
            morgothd->configuration()->readConfig();
            break;
        }

        default:
            qInfo("Caught unhandled signal (%d)", signal);
    }

    m_signal->setEnabled(true);
}

} // namespace morgoth
