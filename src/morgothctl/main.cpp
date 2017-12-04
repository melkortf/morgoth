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
#include "serverconfigurationinterface.h"
#include "servercoordinatorinterface.h"
#include "servermanagerinterface.h"
#include <QtCore>
#include <algorithm>
#include <cstdio>

static const QString morgothService = morgoth::MorgothDaemon::dbusServiceName();
static QDBusConnection dbus = QDBusConnection::sessionBus();
static QTextStream qstdout(stdout);
static org::morgoth::ServerManager* serverManager;

void list(const QStringList& args)
{
    Q_UNUSED(args);

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

int start(const QStringList& arguments)
{
    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.setApplicationDescription("start - start a server");
    parser.addHelpOption();
    parser.addPositionalArgument("name", "The server name");
    parser.process(arguments);

    QStringList args = parser.positionalArguments();
    if (args.isEmpty())
        parser.showHelp();

    QStringList servers = serverManager->servers();
    int val = 0;
    for (const QString& serverName: qAsConst(args)) {
        if (!servers.contains(serverName)) {
            qstdout << "Server \"" << serverName << "\" does not exist" << endl;
            val += 1;
            continue;
        }

        QString path = QStringLiteral("/servers/%1").arg(serverName);
        org::morgoth::Server* server = new org::morgoth::Server(morgothService, path, dbus, qApp);
        if (!server->valid()) {
            qstdout << "Server \"" << serverName << "\" is invalid" << endl;
            val += 1;
            continue;
        }

        org::morgoth::ServerCoordinator* coordinator =
                new org::morgoth::ServerCoordinator(morgothService, path + "/coordinator", dbus, qApp);
        bool ret = coordinator->start();
        val += ret ? 0 : 1;
    }

    return val;
}

int stop(const QStringList& arguments)
{
    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.setApplicationDescription("stop - stop a server");
    parser.addHelpOption();
    parser.addPositionalArgument("name", "The server name");
    parser.process(arguments);

    QStringList args = parser.positionalArguments();
    if (args.isEmpty())
        parser.showHelp();

    QStringList servers = serverManager->servers();
    int val = 0;
    for (const QString& serverName: qAsConst(args)) {
        if (!servers.contains(serverName)) {
            qstdout << "Server \"" << serverName << "\" does not exist" << endl;
            val += 1;
            continue;
        }

        QString path = QStringLiteral("/servers/%1").arg(serverName);
        org::morgoth::Server* server = new org::morgoth::Server(morgothService, path, dbus, qApp);
        if (!server->valid()) {
            qstdout << "Server \"" << serverName << "\" is invalid" << endl;
            val += 1;
            continue;
        }

        org::morgoth::ServerCoordinator* coordinator =
                new org::morgoth::ServerCoordinator(morgothService, path + "/coordinator", dbus, qApp);
        coordinator->stop();
    }

    return val;
}

int config(const QStringList& arguments)
{
    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.setApplicationDescription("config - set/get a server's configuration");
    parser.addHelpOption();
    parser.addPositionalArgument("name", "The server name");
    parser.addPositionalArgument("key", "The config key");
    parser.addPositionalArgument("value", "The config value");
    parser.process(arguments);

    QStringList args = parser.positionalArguments();
    if (args.isEmpty())
        parser.showHelp();

    QString server = args.at(0);
    if  (!serverManager->servers().contains(server)) {
        qstdout << "Server \"" << server << "\" does not exist" << endl;
        return 1;
    }

    QString path = QStringLiteral("/servers/%1/configuration").arg(server);
    org::morgoth::ServerConfiguration* configuration =
            new org::morgoth::ServerConfiguration(morgothService, path, dbus, qApp);

    QStringList keys = configuration->keys();

    if (args.size() == 1) {
        // list the whole configuration
        int padding = std::max_element(keys.begin(), keys.end(), [](auto a, auto b) { return a.size() < b.size(); })->size();
        for (const QString& key: qAsConst(keys)) {
            qstdout << qSetFieldWidth(padding) << right << key << qSetFieldWidth(0) << ": " << left << configuration->value(key) << endl;
        }
    } else if (args.size() == 2) {
        QString key = args.at(1);
        qstdout << configuration->value(key) << endl;
    } else {
        QString key = args.at(1);
        QString value = args.at(2);
        configuration->setValue(key, value);
    }

    return 0;
}

int add(const QStringList& arguments)
{
    QCommandLineParser parser;
    parser.setOptionsAfterPositionalArgumentsMode(QCommandLineParser::ParseAsPositionalArguments);
    parser.setApplicationDescription("add - add a new server instance");
    parser.addHelpOption();
    parser.addPositionalArgument("path", "The server installation path");
    parser.addPositionalArgument("name", "The new server's name");
    parser.process(arguments);

    QStringList args = parser.positionalArguments();
    if (args.size() != 2)
        parser.showHelp();

    QString path = args.at(0);
    QString name = args.at(1);
    bool ret = serverManager->add(path, name);
    return ret ? 0 : 1;
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

    QStringList args = parser.positionalArguments();
    if (args.isEmpty())
        parser.showHelp();

    if (!findServerManager()) {
        qstdout << "Couldn't access morgoth daemon over its D-Bus interface; is morgothd running?" << endl;
        return 1;
    }

    int ret = 0;
    QString cmd = args.at(0);

    if (cmd == "list") {
        list(args);
    } else if (cmd == "start") {
        ret = start(args);
    } else if (cmd == "stop") {
        ret = stop(args);
    } else if (cmd == "config") {
        ret = config(args);
    } else if (cmd == "add") {
        ret = add(args);
    }

    return ret;
}
