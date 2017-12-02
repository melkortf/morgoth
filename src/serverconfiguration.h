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

#ifndef SERVERCONFIGURATION_H
#define SERVERCONFIGURATION_H

#include "morgoth_export.h"
#include <QtCore/QObject>
#include <QtCore/QMap>

namespace morgoth {

class Server;

class MORGOTH_EXPORT ServerConfiguration : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.ServerConfiguration")

    Q_PROPERTY(QStringList keys READ keys)

signals:
    void valueChanged(QString key, QString newValue);

public:
    explicit ServerConfiguration(Server* server);

    QStringList keys() const { return m_values.keys(); }

    const Server* server() const { return m_server; }

public slots:
    void setValue(const QString& key, const QString& value);
    QString value(const QString& key) const;

private:
    const Server* m_server;
    QMap<QString, QString> m_values;

};

} // namespace morgoth

#endif // SERVERCONFIGURATION_H
