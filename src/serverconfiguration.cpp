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

#include "serverconfiguration.h"
#include "morgothdaemon.h"
#include "server.h"
#include "serverconfigurationadaptor.h"

namespace morgoth {

ServerConfiguration::ServerConfiguration(Server* server) :
    QObject(server),
    m_server(server)
{
    new ServerConfigurationAdaptor(this);
    morgothd->dbusConnection().registerObject(server->configurationPath().path(), this);
}

void ServerConfiguration::setValue(const QString& key, const QString& value)
{
    QString oldValue = m_values.value(key);
    if (oldValue != value) {
        m_values.insert(key, value);
        emit valueChanged(key, value);
    }
}

QString ServerConfiguration::value(const QString& key) const
{
    return m_values.value(key);
}

} // namespace morgoth
