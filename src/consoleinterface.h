#ifndef CONSOLEINTERFACE_H
#define CONSOLEINTERFACE_H

#include "servermanager.h"
#include <QObject>

namespace morgoth {

class StdinReader;

class ConsoleInterface : public QObject {
    Q_OBJECT

public:
    explicit ConsoleInterface(ServerManager* serverManager, QObject *parent = nullptr);

private:
    void handleServerCommand(const QString& serverName, const QString& action);

private slots:
    void handleCommand(QString line);

private:
    ServerManager* m_serverManager;
    StdinReader* m_stdinReader;

};

} // namespace morgoth

#endif // CONSOLEINTERFACE_H
