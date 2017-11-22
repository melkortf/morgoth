#include "server.h"
#include "servercoordinator.h"
#include "dbus/serveradaptor.h"
#include <QtCore>

namespace morgoth {

Server::Server(const QString& path, const QString& name, QObject* parent) :
    QObject(parent),
    m_name(name),
    m_path(path),
    m_srcdsExec(path + "/srcds_run"),
    m_coordinator(new ServerCoordinator(this))
{
    discover();
    new dbus::ServerAdaptor(this);
}

Server::~Server() {}

void Server::discover()
{
    m_valid = QFile::exists(m_srcdsExec);
}

} // namespace Morgoth
