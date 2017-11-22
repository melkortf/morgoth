#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QtCore/QObject>
#include <QtCore/QMap>
#include <QtCore/QVariant>

namespace morgoth {

class MorgothDaemon;

class Configuration : public QObject {
    Q_OBJECT

signals:
    void reloaded();

public:
    explicit Configuration(const QString& configFileName = QString(), QObject *parent = nullptr);

    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

private:
    friend class MorgothDaemon;
    void readConfig();
    void loadDefaults();

    QMap<QString, QVariant> m_config;
    const QString& m_configFileName;

};

} // namespace morgoth

#endif // CONFIGURATION_H
