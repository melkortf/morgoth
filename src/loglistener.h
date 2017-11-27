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

#ifndef LOGLISTENER_H
#define LOGLISTENER_H

#include "eventhandler.h"
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QThread>

namespace morgoth {

/**
 * \brief The LogListener class reads server log line by line.
 * \note The log file is supposed to be a fifo one.
 * \sa EventHandler.
 */
class LogListener : public QThread {
    Q_OBJECT

public:
    /**
     * \brief Creates a new LogListener instance.
     * \param filePath Path to the log file.
     * \param parent Passed to QObject.
     */
    explicit LogListener(const QString& filePath, QObject* parent = nullptr);

    /**
     * \brief Attaches a new \c EventHandler to this LogListener.
     * \param handler The \c EventHandler instance to be installed.
     */
    void installEventHandler(EventHandler* handler);

    /**
     * \brief Returns path to the log file.
     */
    const QString& filePath() const { return m_filePath; }

protected:
    /**
     * \copydoc QThread::run()
     */
    void run() override;

private:
    QString m_filePath;
    bool m_isFinished = false;
    QList<EventHandler*> m_events;
    QMutex m_eventListMutex;

};

} // namespace Morgoth

#endif // LOGLISTENER_H
