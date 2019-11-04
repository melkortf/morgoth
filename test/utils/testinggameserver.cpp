#include "gameservermock.h"
#include <QtCore>
#include <iostream>
#include <string>

GameServerMock gameServer;

void readCommand()
{
    std::string line;
    std::getline(std::cin, line);
    QString strLine = QString::fromStdString(line);
    auto parts = strLine.split(' ');

    QString command = parts.takeFirst();
    if (command == QStringLiteral("quit")) {
        qApp->quit();
    } else if (command == QStringLiteral("connect")) {
        gameServer.connect(parts.first());
    } else if (command == QStringLiteral("set_maxplayers")) {
        gameServer.setMaxPlayers(parts.first().toInt());
    } else if (command == QStringLiteral("set_address")) {
        gameServer.setAddress(parts.first());
    } else {
        qWarning() << "unrecognized command";
    }
}

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (app.arguments().length() < 2) {
        qFatal("server path not specified");
        return 1;
    }

    gameServer.setGameLocation(app.arguments().at(1));

    QSocketNotifier notifier(fileno(stdin), QSocketNotifier::Read);
    QObject::connect(&notifier, &QSocketNotifier::activated, &readCommand);
    return app.exec();
}
