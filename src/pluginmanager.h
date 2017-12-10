#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "morgoth_export.h"
#include "morgothplugin.h"
#include <QtCore/QDir>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>

namespace morgoth {

class MORGOTH_EXPORT PluginManager : public QObject {
    Q_OBJECT

public:
    explicit PluginManager(QObject *parent = nullptr);

    virtual ~PluginManager() = default;

    void addPluginsDir(const QString& path);

private:
    void readPlugin(const QString& path);

    struct PluginData {
        QString name;
        QString filePath;
        QJsonObject metaData;
        MorgothPlugin* instance;
    };

    QList<PluginData> m_plugins;

};

} // namespace morgoth

#endif // PLUGINMANAGER_H
