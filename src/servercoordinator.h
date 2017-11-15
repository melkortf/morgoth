#ifndef SERVERCOORDINATOR_H
#define SERVERCOORDINATOR_H

#include "eventhandler.h"
#include "server.h"
#include "tmuxsessionwrapper.h"
#include <QtCore/QObject>

namespace Morgoth {

class LogListener;

class ServerCoordinator : public QObject {
    Q_OBJECT

public:
    ServerCoordinator(const Server* server);
    virtual ~ServerCoordinator();

    void installEventHandler(EventHandler* handler);

    const Server* server() const { return m_server; }
    bool isStarted() const { return m_started; }

    EventHandler* findEvent(const QString& name);

public slots:
    bool start();
    void stop();
    bool command(const QString& cmd);

private slots:
    void handleServerStarted();
    void handleServerStopped();

private:
    const Server* m_server;
    TmuxSessionWrapper m_tmux;
    bool m_started = false;
    QString m_logFileName;
    LogListener* m_logListener = nullptr;
    QMap<QString, EventHandler*> m_eventHandlers;

};

} // namespace Morgoth

#endif // SERVERCOORDINATOR_H
