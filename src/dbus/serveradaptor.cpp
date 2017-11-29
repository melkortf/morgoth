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

#include "serveradaptor.h"
#include "../morgothdaemon.h"
#include <QtCore>
#include <QtDBus>

namespace morgoth { namespace dbus {

ServerAdaptor::ServerAdaptor(Server* server) :
    QDBusAbstractAdaptor(server),
    m_server(server)
{
    connect(m_server->coordinator(), &ServerCoordinator::statusChanged,
            this, &ServerAdaptor::handleServerStatusChange);

    QString path = QString("/servers/%1").arg(m_server->name());
    qApp->dbusConnection()->registerObject(path, m_server);
}

QString ServerAdaptor::name() const
{
    return m_server->name();
}

QString ServerAdaptor::path() const
{
    return m_server->path().toString();
}

ServerLaunchArguments ServerAdaptor::launchArguments() const
{
    return m_server->launchArguments();
}

void ServerAdaptor::setLaunchArguments(const ServerLaunchArguments& launchArguments)
{
    m_server->setLaunchArguments(launchArguments);
}

bool ServerAdaptor::isValid() const
{
    return m_server->isValid();
}

QString ServerAdaptor::status() const
{
    return QMetaEnum::fromType<ServerCoordinator::Status>().valueToKey(m_server->coordinator()->status());
}

void ServerAdaptor::start()
{
    m_server->coordinator()->start();
}

void ServerAdaptor::stop()
{
    m_server->coordinator()->stop();
}

void ServerAdaptor::handleServerStatusChange(ServerCoordinator::Status status)
{
    Q_UNUSED(status);
    emit statusChanged(this->status());
}

}} // namespace morgoth::dbus

QDBusArgument& operator<<(QDBusArgument& argument, const morgoth::ServerLaunchArguments& launchArgs)
{
    argument.beginMap(QVariant::String, QVariant::String);

    argument.beginMapEntry();
    argument << QStringLiteral("secured") << (launchArgs.isSecured() ? QString("true") : QString("false"));
    argument.endMapEntry();

    argument.beginMapEntry();
    argument << QStringLiteral("port") << QString::number(launchArgs.port());
    argument.endMapEntry();

    argument.beginMapEntry();
    argument << QStringLiteral("map") << launchArgs.initialMap();
    argument.endMapEntry();

    argument.endMap();
    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, morgoth::ServerLaunchArguments& launchArgs)
{
    argument.beginMap();
    while (!argument.atEnd()) {
        QString key, value;
        argument.beginMapEntry();
        argument >> key >> value;
        argument.endMapEntry();

        if (key == QStringLiteral("secured")) {
            launchArgs.setSecured(value == QStringLiteral("true"));
        } else if (key == QStringLiteral("port")) {
            launchArgs.setPort(value.toUInt());
        } else if (key == QStringLiteral("map")) {
            launchArgs.setInitialMap(value);
        }
    }

    argument.endMap();
    return argument;
}

static void registerDBusMetaType()
{
    qDBusRegisterMetaType<morgoth::ServerLaunchArguments>();
}
Q_COREAPP_STARTUP_FUNCTION(registerDBusMetaType)
