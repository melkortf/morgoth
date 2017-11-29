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

#ifndef SERVERMANAGERADAPTOR_H
#define SERVERMANAGERADAPTOR_H

#include "../servermanager.h"
#include <QtCore/QStringList>
#include <QtDBus/QDBusAbstractAdaptor>

namespace morgoth { namespace dbus {

class ServerManagerAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.ServerManager")
    Q_PROPERTY(QStringList servers READ servers)

signals:
    void serverAdded(const QString& serverName);

public:
    explicit ServerManagerAdaptor(ServerManager* serverManager);

    virtual ~ServerManagerAdaptor();

    QStringList servers() const;

public slots:
    bool add(QString path, QString name);

private slots:
    void handleNewServer(Server* server);

private:
    ServerManager* m_serverManager;

};

}} // namespace morgoth::dbus

#endif // SERVERMANAGERADAPTOR_H
