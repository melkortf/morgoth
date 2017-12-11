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

#include "servermanager.h"
#include "morgothdaemon.h"
#include "servermanageradaptor.h"
#include <QtCore>
#include <algorithm>
#include <iostream>

namespace morgoth {

ServerManager::ServerManager(QObject* parent) :
    QObject(parent)
{
    new ServerManagerAdaptor(this);
    morgothd->dbusConnection().registerObject("/servers", this);
}

Server* ServerManager::find(const QString& name) const
{
    auto it = std::find_if(m_servers.begin(), m_servers.end(), [&name](auto s) {
        return s->name() == name;
    });
    return it == m_servers.end() ? nullptr : *it;
}

Server* ServerManager::add(const QUrl& path, const QString& name)
{
    if (find(name) != nullptr) {
        qWarning("Could not add server \"%s\": name already exists", qPrintable(name));
        return nullptr;
    }

    QUrl fixedPath(path);
    if (QFile::exists(fixedPath.toString())) {
        fixedPath = QUrl::fromLocalFile(fixedPath.toString());
    }

    Server* s = new Server(fixedPath, name, this);
    m_servers.append(s);
    emit serverAdded(s);
    return s;
}

QStringList ServerManager::serverNames() const
{
    QStringList result;
    std::for_each(m_servers.begin(), m_servers.end(), [&result](const Server* s) {
        result.append(s->name());
    });

    return result;
}

} // namespace Morgoth
