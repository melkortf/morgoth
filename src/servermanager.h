#ifndef SERVERMANAGER_H
#define SERVERMANAGER_H

#include "server.h"
#include <QtCore/QObject>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlTableModel>

namespace morgoth {

class ServerManager : public QObject {
    Q_OBJECT

signals:
    void serverAdded(Server* server);
    void serverDeleted(Server* server);

public:
    explicit ServerManager(QObject* parent = nullptr);

    Server* find(const QString& name) const;

    bool add(const QString& path, const QString& name);

    const QList<Server*>& servers() const { return m_servers; }

private:
    void initializeServers();

    QList<Server*> m_servers;
    QSqlDatabase m_database;
    QSqlTableModel m_model;

};

} // namespace Morgoth

#endif // SERVERMANAGER_H
