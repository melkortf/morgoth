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

ServerLaunchArguments::ServerLaunchArguments(const ServerLaunchArguments& other) :
    d(other.d) {}

ServerLaunchArguments::~ServerLaunchArguments() {}

ServerLaunchArguments& ServerLaunchArguments::operator=(const ServerLaunchArguments& other)
{
    d = other.d;
    return *this;
}

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

    return arguments;
}

ServerLaunchArguments ServerLaunchArguments::fromSrcdsArguments(const QString& srcdsArguments)
{
    ServerLaunchArguments result;
    QStringList tmp = srcdsArguments.split(QRegularExpression("\\s+"));
    for (int i = 0; i < tmp.size(); ++i) {
        QString arg = tmp.at(i);
        if (arg == QStringLiteral("-secured"))
            result.setSecured(true);
        else if (arg == QStringLiteral("-insecure"))
            result.setSecured(false);
        else if (arg == QStringLiteral("-port")) {
            unsigned port = tmp.at(++i).toUInt();
            result.setPort(port);
        } else if (arg == QStringLiteral("+map")) {
            QString map = tmp.at(++i);
            result.setInitialMap(map);
        }
    }

    return result;
}

} // namespace morgoth

QDebug operator<<(QDebug dbg, const morgoth::ServerLaunchArguments launchArguments)
{
    dbg.nospace().noquote()
            << "ServerLaunchArguments("
            << launchArguments.asSrcdsArguments()
            << ")";
    return dbg.maybeSpace();
}

static void registerMetaType()
{
    qRegisterMetaType<morgoth::ServerLaunchArguments>();
}

Q_COREAPP_STARTUP_FUNCTION(registerMetaType)
