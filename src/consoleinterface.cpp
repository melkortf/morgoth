#include "consoleinterface.h"
#include "server.h"
#include "servercoordinator.h"
#include "stdinreader.h"
#include <QtCore>

namespace morgoth {

ConsoleInterface::ConsoleInterface(ServerManager* serverManager, QObject *parent) :
    QObject(parent),
    m_serverManager(serverManager),
    m_stdinReader(new StdinReader(this))
{
    Q_ASSERT(m_serverManager);

    QObject::connect(m_stdinReader, &StdinReader::lineReceived, this, &ConsoleInterface::handleCommand);
}

void ConsoleInterface::handleServerCommand(const QString& serverName, const QString& action)
{
    Server* server = m_serverManager->find(serverName);
    if (!server) {
        qWarning("Server %s not found", qPrintable(serverName));
        return;
    }

    if (action == "start")
        server->coordinator()->start();
    else if  (action == "stop")
        server->coordinator()->stop();
}

void ConsoleInterface::handleCommand(QString line)
{
    if (line == "quit") {
        QCoreApplication::quit();
    } else if (line == "list") {
        QTextStream out(stdout);

        for (const Server* s: m_serverManager->servers()) {
            out << s->name() << " (" << s->path().toString() << "): "
                << QMetaEnum::fromType<ServerCoordinator::Status>().valueToKey(s->coordinator()->status())
                << endl;
        }
    } else {
        QRegularExpression serverAction("^(.[^\\s]+)\\s(.+)$");
        auto match = serverAction.match(line);
        if (match.hasMatch()) {
            QString first = match.captured(1);
            if (first == "add") {
                QString args = match.captured(2);
                QStringList splitted = args.split(QRegularExpression("\\s+"));
                if (splitted.size() != 2) {
                    qWarning("Invalid command");
                    return;
                }

                QString name = splitted.at(0);
                QString path = splitted.at(1);
                m_serverManager->add(path, name);
            } else {
                handleServerCommand(match.captured(1), match.captured(2));
            }
        }
    }
}

} // namespace morgoth
