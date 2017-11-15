#include "loglistener.h"
#include <QtCore>
#include <fstream>
#include <string>

namespace Morgoth {

LogListener::LogListener(const QString& filePath, QObject* parent) :
    QThread(parent),
    m_filePath(filePath) {}

void LogListener::installEventHandler(EventHandler *handler)
{
    handler->setParent(this);

    QMutexLocker ml(&m_eventListMutex);
    m_events.append(handler);
}

void LogListener::run()
{
    /* We don't use Qt's API when it comes to using fifo files, due to
     * a couple of bugs, including this one, created in 2010:
     * https://bugreports.qt.io/browse/QTBUG-15261
     */

    std::ifstream fifo(m_filePath.toStdString());
    if (!fifo.is_open()) {
        qWarning("Could not open %s for reading; the LogScanner will not work", qPrintable(m_filePath));
        return;
    }

    std::string line;
    while (!isInterruptionRequested() && std::getline(fifo, line)) {
        QString qtline = QString::fromStdString(line).trimmed();

        QMutexLocker ml(&m_eventListMutex);
        for (auto e: m_events) {
            QRegularExpression regex = e->regex();
            QRegularExpressionMatch match = regex.match(qtline);
            if (match.hasMatch())
                e->maybeActivated(qtline, match);
        }

        QRegularExpression rx("Server Quit$");
        if (rx.match(qtline).hasMatch())
            break;
    }
}

} // namespace Morgoth
