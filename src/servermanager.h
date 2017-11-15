#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include "server.h"
#include <QtCore/QObject>

namespace Morgoth {

class ServerManager : public QObject {
    Q_OBJECT

public:
    ServerManager(QObject* parent = nullptr);

    Server* find(const QString& name) const;

private:
    void initializeServers();

    QList<Server*> m_servers;

};

} // namespace Morgoth

#endif // SERVERMANAGER_H
