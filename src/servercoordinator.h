#ifndef SERVERCOORDINATOR_H
#define SERVERCOORDINATOR_H

#include "eventhandler.h"
#include "server.h"
#include "tmuxsessionwrapper.h"
#include <QtCore/QObject>

namespace morgoth {

class LogListener;

class ServerCoordinator : public QObject {
    Q_OBJECT
    Q_PROPERTY(ServerCoordinator::Status status READ status NOTIFY statusChanged)

public:
    enum Status { Offline, Starting, Running, ShuttingDown, Crashed };
    Q_ENUM(Status)

signals:
    void statusChanged(ServerCoordinator::Status status);

public:
    explicit ServerCoordinator(const Server* server);
    virtual ~ServerCoordinator();

    void installEventHandler(EventHandler* handler);

    const Server* server() const { return m_server; }
    Status status() const { return m_status; }

    EventHandler* findEvent(const QString& name);

public slots:
    void setStatus(Status status);
    bool start();
    void stop();
    bool command(const QString& cmd);

private slots:
    void handleServerStarted();
    void handleServerStopped();
    void stopSync();

private:
    const Server* m_server;
    Status m_status = Status::Offline;
    TmuxSessionWrapper m_tmux;
    QString m_logFileName;
    LogListener* m_logListener = nullptr;
    QMap<QString, EventHandler*> m_eventHandlers;

};

} // namespace Morgoth

#endif // SERVERCOORDINATOR_H
