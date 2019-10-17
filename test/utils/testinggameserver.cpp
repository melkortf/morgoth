#include "gameservermock.h"
#include <QtCore>

int main(int argc, char** argv)
{
    QCoreApplication app(argc, argv);

    if (app.arguments().length() < 3) {
        qFatal("no D-Bus server specified");
        return 1;
    }

    GameServerMock gameServer;
    gameServer.setGameLocation(app.arguments().at(2));
    gameServer.connect(app.arguments().at(1));

    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &app, &QCoreApplication::quit);
    timer.start(1000);
    return app.exec();
}
