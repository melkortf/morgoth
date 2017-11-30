#include "config.h"
#include "serverinterface.h"
#include "servercoordinatorinterface.h"
#include "servermanagerinterface.h"
#include <cstdio>
#include <QtCore>

static const QString morgothService = QStringLiteral("org.morgoth");
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

        qstdout << server->name() << " (" << server->path() << "): " << status << endl;
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

    serverManager = new org::morgoth::ServerManager(morgothService, "/serverManager", dbus, qApp);
    if (!serverManager->isValid()) {
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
