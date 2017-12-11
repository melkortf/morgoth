#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "morgoth_export.h"
#include "morgothplugin.h"
#include <QtCore/QDir>
#include <QtCore/QList>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>

class QPluginLoader;

namespace morgoth {

/**
 * \brief The PluginManager class is responsible for controlling all of morgoth
 *  plugins.
 */
class MORGOTH_EXPORT PluginManager : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.PluginManager")

    /**
     * Holds list of all available (enabled and disabled) plugins.
     */
    Q_PROPERTY(QStringList availablePlugins READ availablePlugins)

signals:
    /**
     * \brief This signal is emitted everytime a single plugin is enabled or disabled.
     */
    void pluginStatusChanged(const QString& pluginName, bool enabled);

public:
    /**
     * \brief Creates a new \c PluginManager instance.
     * \param parent Passed to QObject.
     */
    explicit PluginManager(QObject *parent = nullptr);

    virtual ~PluginManager() = default;

    QStringList availablePlugins() const;

public slots:
    /**
     * \brief Searches recursively for plugins.
     *
     * The \c path is scanned recursively for existing plugins. When a plugin
     * is found, it is added to the list of available ones. No plugin is
     * enabled by default.
     *
     * \param path The directory to scan.
     */
    void addPluginsDir(const QString& path);

    /**
     * \brief Enabled or disables the given plugin.
     * \param name Name of the plugin.
     * \param enabled Whether to enable or disable the plugin.
     */
    void setPluginStatus(const QString& name, bool enabled);

private:
    bool hasValidMetadata(QPluginLoader* loader, QString* name);
    void readPlugin(const QString& path);
    void loadPlugin(const QString& name);
    void unloadPlugin(const QString& name);

    struct PluginData {
        QString name;
        QPluginLoader* loader;
    };

    QList<PluginData> m_plugins;

};

} // namespace morgoth

#endif // PLUGINMANAGER_H
