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

#include "logcollector.h"
#include <QtCore>

namespace morgoth {

class LogCollectorPrivate {
public:
    LogCollectorPrivate(const QString& directory);

    QMutex mutex;
    QString directory;
    QByteArray data;
    bool active;
};

LogCollectorPrivate::LogCollectorPrivate(const QString &directory) :
    directory(directory)
{

}

LogCollector::LogCollector(const QString& directory, QObject *parent) :
    QObject(parent),
    d(new LogCollectorPrivate(directory)) {}

LogCollector::~LogCollector()
{
    save();
}

void LogCollector::log(const QString& line)
{
    d->mutex.lock();
    d->data.append(line).append("\n");
    d->mutex.unlock();

    if (d->data.size() >= MaxLogSize())
        save();
}

void LogCollector::setDirectory(const QString& directory)
{
    if (d->directory != directory) {
        if (!d->directory.isEmpty())
            save();

        d->directory = directory;
    }
}

const QString &LogCollector::directory() const
{
    return d->directory;
}

void LogCollector::save()
{
    if (d->data.isEmpty())
        return;

    if (!isLogDirWritable()) {
        qWarning("%s: log directory %s not accessible", Q_FUNC_INFO, qPrintable(d->directory));
        return;
    }

    QFile output(logFileName());
    if (output.open(QIODevice::Append)) {
        QMutexLocker l(&d->mutex);
        output.write(d->data);
        output.close();
        d->data.clear();
        qDebug("Logs saved to %s", qPrintable(output.fileName()));
    } else {
        qWarning("Could not open %s for writing", qPrintable(output.fileName()));
    }
}

bool LogCollector::isLogDirWritable()
{
    if (!QDir(d->directory).exists()) {
        bool result = QDir().mkpath(d->directory);
        if (!result) {
            return false;
        }
    }

    return QDir(d->directory).isReadable();
}

QString LogCollector::logFileName()
{
    QString fileName = QDate::currentDate().toString("yyyyMMdd").append(".log");
    return QDir(d->directory).absoluteFilePath(fileName);
}

} // namespace morgoth
