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
