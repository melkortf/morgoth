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

LogListener::LogListener(const QString& filePath, QObject* parent) :
    QThread(parent),
    m_filePath(filePath) {}

void LogListener::installEventHandler(EventHandler *handler)
{
    handler->setParent(this);

    QMutexLocker ml(&m_eventListMutex);
    m_events.append(handler);
}

void LogListener::setLogCollector(LogCollector* logCollector)
{
    m_logCollector = logCollector;
}

void LogListener::run()
{
    /* We don't use Qt's API when it comes to using fifo files, due to
     * a couple of bugs, including this one, reported back in 2010:
     * https://bugreports.qt.io/browse/QTBUG-15261
     */

    std::ifstream fifo(m_filePath.toStdString());
    if (!fifo.is_open()) {
        qWarning("Could not open %s for reading; the LogScanner will not work", qPrintable(m_filePath));
        return;
    }

    static QRegularExpression quitRx("Server Quit$");

    std::string line;
    while (!isInterruptionRequested() && std::getline(fifo, line)) {
        QString qtline = QString::fromStdString(line).trimmed();
        if (m_logCollector)
            m_logCollector->log(qtline);

        QMutexLocker ml(&m_eventListMutex);
        for (auto e: m_events) {
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
