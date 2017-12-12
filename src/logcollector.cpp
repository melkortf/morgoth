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

LogCollector::LogCollector(const QString& directory, QObject *parent) :
    QObject(parent),
    m_directory(directory) {}

LogCollector::~LogCollector()
{
    save();
}

void LogCollector::log(const QString& line)
{
    m_mutex.lock();
    m_data.append(line).append("\n");
    m_mutex.unlock();

    if (m_data.size() >= MaxLogSize())
        save();
}

void LogCollector::setDirectory(const QString& directory)
{
    if(m_directory != directory) {
        if (!m_directory.isEmpty())
            save();

        m_directory = directory;
    }
}

void LogCollector::save()
{
    if (m_data.isEmpty())
        return;

    if (!isLogDirWritable()) {
        qWarning("%s: log directory %s not accessible", Q_FUNC_INFO, qPrintable(m_directory));
        return;
    }

    QFile output(logFileName());
    if (output.open(QIODevice::Append)) {
        QMutexLocker l(&m_mutex);
        output.write(m_data);
        output.close();
        m_data.clear();
        qDebug("Logs saved to %s", qPrintable(output.fileName()));
    } else {
        qWarning("Could not open %s for writing", qPrintable(output.fileName()));
    }
}

bool LogCollector::isLogDirWritable()
{
    if (!QDir(m_directory).exists()) {
        bool result = QDir().mkpath(m_directory);
        if (!result) {
            return false;
        }
    }

    return QDir(m_directory).isReadable();
}

QString LogCollector::logFileName()
{
    QString fileName = QDate::currentDate().toString("yyyyMMdd").append(".log");
    return QDir(m_directory).absoluteFilePath(fileName);
}

} // namespace morgoth
