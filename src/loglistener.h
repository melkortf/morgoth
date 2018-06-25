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

#include "gameevent.h"
#include "logcollector.h"
#include "morgoth_export.h"
#include <QtCore/QList>
#include <QtCore/QMutex>
#include <QtCore/QThread>

namespace morgoth {

class LogListenerPrivate;

/**
 * \brief The LogListener class reads server log line by line.
 * \note The log file is supposed to be a fifo one.
 * \sa GameEvent.
 */
class MORGOTH_EXPORT LogListener : public QThread {
    Q_OBJECT

public:
    /**
     * \brief Creates a new LogListener instance.
     * \param filePath Path to the log file.
     * \param parent Passed to QObject.
     */
    explicit LogListener(const QString& filePath, QObject* parent = nullptr);

    /**
     * Destroys this \c LogListener instance.
     */
    virtual ~LogListener();

    /**
     * \brief Attaches a new \c GameEvent to this LogListener.
     * \param handler The \c GameEvent instance to be installed.
     */
    void installGameEvent(GameEvent* handler);

    /**
     * \brief Returns path to the log file.
     */
    const QString& filePath() const;

    /**
     * \brief Sets the LogCollector instance.
     */
    void setLogCollector(LogCollector* logCollector);

protected:
    /**
     * \copydoc QThread::run()
     */
    void run() override;

private:
    QScopedPointer<LogListenerPrivate> d;

};

} // namespace Morgoth

#endif // LOGLISTENER_H
