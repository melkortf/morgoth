#include "serverlauncharguments.h"
#include <QtCore>

namespace morgoth {

class ServerLaunchArgumentsData : public QSharedData {
public:
    ServerLaunchArgumentsData() : port(27015), secured(true) {}
    ServerLaunchArgumentsData(const ServerLaunchArgumentsData& other) :
        QSharedData(other), port(other.port), secured(other.secured),
        initialMap(other.initialMap) {}
    ~ServerLaunchArgumentsData() {}

    unsigned port;
    bool secured;
    QString initialMap;
};

ServerLaunchArguments::ServerLaunchArguments() :
    d(new ServerLaunchArgumentsData) {}

ServerLaunchArguments::~ServerLaunchArguments() {}

unsigned ServerLaunchArguments::port() const
{
    return d->port;
}

void ServerLaunchArguments::setPort(unsigned port)
{
    d->port = port;
}

bool ServerLaunchArguments::isSecured() const
{
    return d->secured;
}

void ServerLaunchArguments::setSecured(bool secured)
{
    d->secured = secured;
}

QString ServerLaunchArguments::initialMap() const
{
    return d->initialMap;
}

void ServerLaunchArguments::setInitialMap(const QString& initialMap)
{
    d->initialMap = initialMap;
}

QString ServerLaunchArguments::asSrcdsArguments() const
{
    QString arguments = QStringLiteral("-port %1 -%2")
            .arg(QString::number(port()), isSecured() ? "secured" : "insecure");
    QString initialMapArg = initialMap();
    if (!initialMapArg.isEmpty()) {
        arguments.append(QStringLiteral(" +map %1").arg(initialMapArg));
    }

    qDebug("srcds_run %s", qPrintable(arguments));
    return arguments;
}

} // namespace morgoth
