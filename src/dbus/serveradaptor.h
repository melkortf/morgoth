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

#ifndef SERVERADAPTOR_H
#define SERVERADAPTOR_H

#include "../serverlauncharguments.h"
#include "../server.h"
#include "../servercoordinator.h"
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusArgument>

namespace morgoth { class MorgothDaemon; }

namespace morgoth { namespace dbus {

class ServerAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.Server")
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString path READ path)
    Q_PROPERTY(bool valid READ isValid)
    Q_PROPERTY(morgoth::ServerLaunchArguments launchArguments READ launchArguments WRITE setLaunchArguments)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

signals:
    void statusChanged(QString newStatus);

public:
    explicit ServerAdaptor(Server* server);

    QString name() const;
    QString path() const;
    ServerLaunchArguments launchArguments() const;
    void setLaunchArguments(const ServerLaunchArguments& launchArguments);
    bool isValid() const;
    QString status() const;

public slots:
    Q_NOREPLY void start();
    Q_NOREPLY void stop();

private slots:
    void handleServerStatusChange(ServerCoordinator::Status status);

private:
    Server* m_server;
    MorgothDaemon* m_morgothDaemon;

};

}} // namespace morgoth::dbus

QDBusArgument& operator<<(QDBusArgument& argument, const morgoth::ServerLaunchArguments& launchArgs);
const QDBusArgument& operator>>(const QDBusArgument& argument, morgoth::ServerLaunchArguments& launchArgs);

#endif // SERVERADAPTOR_H
