// This file is part of morgoth.

// morgoth is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "loglistener.h"
#include <QtCore>
#include <fstream>
#include <string>

namespace morgoth {

class LogListenerPrivate {
public:
    LogListenerPrivate(const QString& filePath);

    QString filePath;
    bool isFinished = false;
    QList<EventHandler*> events;
    QMutex eventListMutex;
    LogCollector* logCollector = nullptr;
};

LogListenerPrivate::LogListenerPrivate(const QString &filePath) :
    filePath(filePath)
{

}

LogListener::LogListener(const QString& filePath, QObject* parent) :
    QThread(parent),
    d(new LogListenerPrivate{filePath})
{

}

LogListener::~LogListener()
{

}

void LogListener::installEventHandler(EventHandler *handler)
{
    handler->setParent(this);

    QMutexLocker ml(&d->eventListMutex);
    d->events.append(handler);
}

const QString& LogListener::filePath() const
{
    return d->filePath;
}

void LogListener::setLogCollector(LogCollector* logCollector)
{
    d->logCollector = logCollector;
}

void LogListener::run()
{
    /* We don't use Qt's API when it comes to using fifo files, due to
     * a couple of bugs, including this one, reported back in 2010:
     * https://bugreports.qt.io/browse/QTBUG-15261
     */

    std::ifstream fifo(d->filePath.toStdString());
    if (!fifo.is_open()) {
        qWarning("Could not open %s for reading; the LogScanner will not work", qPrintable(d->filePath));
        return;
    }

    static QRegularExpression quitRx("Server Quit$");

    std::string line;
    while (!isInterruptionRequested() && std::getline(fifo, line)) {
        QString qtline = QString::fromStdString(line).trimmed();
        if (d->logCollector)
            d->logCollector->log(qtline);

        QMutexLocker ml(&d->eventListMutex);
        for (auto e: d->events) {
            QRegularExpression regex = e->regex();
            QRegularExpressionMatch match = regex.match(qtline);
            if (match.hasMatch())
                e->maybeActivated(qtline, match);
        }

        if (quitRx.match(qtline).hasMatch())
            break;
    }
}

} // namespace Morgoth
