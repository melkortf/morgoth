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

#include "configuration.h"
#include <QtCore>

namespace morgoth {

Configuration::Configuration(const QString& configFileName, QObject *parent) :
    QObject(parent),
    m_configFileName(configFileName)
{
    loadDefaults();
    readConfig();
}

QVariant Configuration::value(const QString& key, const QVariant& defaultValue) const
{
    return m_config.value(key, defaultValue);
}

void Configuration::readConfig()
{
    if (m_configFileName.isEmpty())
        return;

    QSettings s(m_configFileName, QSettings::IniFormat);
    if (s.status() == QSettings::NoError) {
        QStringList keys = s.allKeys();
        for (const QString& key: qAsConst(keys)) {
            QVariant value = s.value(key);
            Q_ASSERT(!value.isNull());

            m_config.insert(key, value);
        }

        qDebug("%s loaded.", qPrintable(m_configFileName));
        emit reloaded();
    } else {
        qWarning() << m_configFileName << ": " << s.status();
    }
}

void Configuration::loadDefaults()
{
    m_config.insert("dbus", "session");
    m_config.insert("database", "morgoth.sqlite");
}

} // namespace morgoth
