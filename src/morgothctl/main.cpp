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

#include "config.h"
#include "morgothdaemon.h"
#include "serverinterface.h"
#include "servercoordinatorinterface.h"
#include "servermanagerinterface.h"
#include <cstdio>
#include <QtCore>

static const QString morgothService = morgoth::MorgothDaemon::dbusServiceName();
static QDBusConnection dbus = QDBusConnection::sessionBus();
static QTextStream qstdout(stdout);
static org::morgoth::ServerManager* serverManager;

void list()
{
    QStringList servers = serverManager->servers();
    for (const QString& s: qAsConst(servers)) {
        QString path = QStringLiteral("/servers/%1").arg(s);
        org::morgoth::Server* server = new org::morgoth::Server(morgothService, path, dbus, qApp);
        if (!server->isValid())
            continue;

        QString status = QStringLiteral("<< invalid >>");
        if (server->valid()) {
            org::morgoth::ServerCoordinator* coordinator =
                    new org::morgoth::ServerCoordinator(morgothService, path + "/coordinator", dbus, qApp);

            status = QMetaEnum::fromType<morgoth::ServerCoordinator::Status>().valueToKey(coordinator->status());
        }

        qstdout << server->name() << ": " << status << endl;
    }
}

int start(const QString& serverName)
{
    QStringList servers = serverManager->servers();
    if (!servers.contains(serverName)) {
        qstdout << "Server \"" << serverName << "\" does not exist" << endl;
        return 1;
    }

    QString path = QStringLiteral("/servers/%1").arg(serverName);
    org::morgoth::Server* server = new org::morgoth::Server(morgothService, path, dbus, qApp);
    if (!server->valid()) {
        qstdout << "Server \"" << serverName << "\" is invalid" << endl;
        return 1;
    }

    org::morgoth::ServerCoordinator* coordinator =
            new org::morgoth::ServerCoordinator(morgothService, path + "/coordinator", dbus, qApp);
    bool ret = coordinator->start();
    return ret ? 0 : 2;
}

int stop(const QString& serverName)
{
    QStringList servers = serverManager->servers();
    if (!servers.contains(serverName)) {
        qstdout << "Server \"" << serverName << "\" does not exist" << endl;
        return 1;
    }

    QString path = QStringLiteral("/servers/%1").arg(serverName);
    org::morgoth::Server* server = new org::morgoth::Server(morgothService, path, dbus, qApp);
    if (!server->valid()) {
        qstdout << "Server \"" << serverName << "\" is invalid" << endl;
        return 1;
    }

    org::morgoth::ServerCoordinator* coordinator =
            new org::morgoth::ServerCoordinator(morgothService, path + "/coordinator", dbus, qApp);
    coordinator->stop();
    return 0;
}

bool findServerManager()
{
    // try to connect to the service on the session bus first, then on the system one
    serverManager = new org::morgoth::ServerManager(morgothService, "/serverManager", dbus, qApp);
    if (!serverManager->isValid()) {
        delete serverManager;

        dbus = QDBusConnection::systemBus();
        serverManager = new org::morgoth::ServerManager(morgothService, "/serverManager", dbus, qApp);

        if (!serverManager->isValid())
            return false;
    }

    return true;
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);
    app.setApplicationName("morgothctl");
    app.setApplicationVersion(MORGOTH_VERSION);

    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.setApplicationDescription("A morgoth control program");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("command", "A command to run");
    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.isEmpty())
        parser.showHelp();

    if (!findServerManager()) {
        qstdout << "Couldn't access morgoth daemon over its D-Bus interface; is morgothd running?" << endl;
        return 1;
    }

    int ret = 0;
    QString cmd = args.at(0);
    if (cmd == "list") {
        list();
    } if (cmd == "start") {
        if (args.size() < 2)
            parser.showHelp(1);

        QString serverName = args.at(1);
        ret = start(serverName);
    } if (cmd == "stop") {
        if (args.size() < 2)
            parser.showHelp(1);

        QString serverName = args.at(1);
        ret = stop(serverName);
    }

    return ret;
}
