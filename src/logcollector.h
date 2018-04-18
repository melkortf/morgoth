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

#ifndef LOGCOLLECTOR_H
#define LOGCOLLECTOR_H

#include "morgoth_export.h"
#include <QtCore/QByteArray>
#include <QtCore/QMutex>
#include <QtCore/QObject>

namespace morgoth {

class LogCollectorPrivate;

/**
 * \brief The LogCollector class is responsible for saving all servers' logs.
 */
class MORGOTH_EXPORT LogCollector : public QObject {
    Q_OBJECT

    /**
     * \brief The directory where all the logs are saved in.
     */
    Q_PROPERTY(QString directory READ directory WRITE setDirectory)

public:
    /**
     * Creates a new \c LogCollector instance that writes logs to the given
     * \c directory.
     */
    explicit LogCollector(const QString& directory, QObject *parent = nullptr);

    /**
     * Destroys this \c LogCollector instance.
     */
    virtual ~LogCollector();

    /**
     * \brief Appends a single line of log.
     * \note This method is thread-safe.
     */
    virtual void log(const QString& line);

    void setDirectory(const QString& directory);
    const QString& directory() const;

    /**
     * \brief Size (in bytes) of logs stored in memory before they get saved
     *  to a file.
     */
    static constexpr int MaxLogSize() { return 64 * 1024; }

public slots:
    /**
     * \brief Saves the current data to a file and clears it.
     * \note This method is thread-safe.
     */
    void save();

protected:
    bool isLogDirWritable();
    virtual QString logFileName();

private:
    QScopedPointer<LogCollectorPrivate> d;

};

} // namespace morgoth

#endif // LOGCOLLECTOR_H
