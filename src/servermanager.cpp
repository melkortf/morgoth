#include "servermanager.h"
#include <algorithm>
#include <iostream>

namespace Morgoth {

ServerManager::ServerManager(QObject* parent) :
    QObject(parent)
{
    initializeServers();
}

Server* ServerManager::find(const QString& name) const
{
    auto it = std::find_if(m_servers.begin(), m_servers.end(), [&name](auto s) {
        return s->name() == name;
    });
    return *it;
}

void ServerManager::initializeServers()
{
    // TODO Read servers from a database

    Server* server = new Server("/home/garapich/tf2/test", "one", this);
    m_servers.push_back(server);

    std::for_each(m_servers.begin(), m_servers.end(), [](auto s) {
        qInfo("%s: %s", qPrintable(s->name()), (s->isValid() ? qPrintable(s->path()) : "NOT FOUND"));
    });
}

} // namespace Morgoth
