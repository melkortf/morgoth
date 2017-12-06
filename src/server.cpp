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

#include "server.h"
#include "morgothdaemon.h"
#include "servercoordinator.h"
#include "serveradaptor.h"
#include <QtCore>

namespace morgoth {

Server::Server(const QUrl& path, const QString& name, QObject* parent) :
    QObject(parent),
    m_name(name),
    m_path(path)
{
    discover();

    new ServerAdaptor(this);

    if (morgothd) {
        QString dbusPath = QStringLiteral("/servers/%1").arg(name);
        morgothd->dbusConnection().registerObject(dbusPath, this);
    }

    m_configurationPath.setPath(QStringLiteral("/servers/%1/configuration").arg(name));
    initializeDefaultConfiguration();

    if (isValid()) {
        m_coordinatorPath.setPath(QStringLiteral("/servers/%1/coordinator").arg(name));
        m_coordinator = new ServerCoordinator(this);
    }
}

Server::~Server() {}

void Server::discover()
{
    m_valid = false;

    if (!m_path.isLocalFile()) {
        qWarning("Cannot handle server %s: remote servers not supported", qPrintable(m_path.toString()));
        return;
    }

    m_srcdsExec = m_path.toLocalFile() + QDir::separator() + "srcds_run";

    if (!QFile::exists(m_srcdsExec)) {
        qWarning("%s does not exist", qPrintable(m_srcdsExec));
        return;
    }

    m_valid = true;
}

void Server::initializeDefaultConfiguration()
{
    m_configuration = new ServerConfiguration(this);
    m_configuration->setValue("org.morgoth.Server.launchArguments", "-port 27015 -secured +map cp_badlands");
}

} // namespace Morgoth
