#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include "server.h"
#include <QtCore/QObject>

namespace morgoth {

class ServerManager : public QObject {
    Q_OBJECT

public:
    explicit ServerManager(QObject* parent = nullptr);

    Server* find(const QString& name) const;

    const QList<Server*>& servers() const { return m_servers; }

private:
    void initializeServers();

    QList<Server*> m_servers;

};

} // namespace Morgoth

#endif // SERVERMANAGER_H
