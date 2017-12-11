#include "pluginmanager.h"
#include "morgothdaemon.h"
#include "pluginmanageradaptor.h"
#include <QtCore>

namespace morgoth {

PluginManager::PluginManager(QObject *parent) :
    QObject(parent)
{
    new PluginManagerAdaptor(this);
    morgothd->dbusConnection().registerObject("/plugins", this);
}

QStringList PluginManager::availablePlugins() const
{
    QStringList res;
    for (const auto& p: m_plugins) {
        res << p.name;
    }

    return res;
}

void PluginManager::addPluginsDir(const QString& path)
{
    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        readPlugin(it.next());
    }
}

void PluginManager::setPluginStatus(const QString& name, bool enabled)
{
    if (enabled) {
        loadPlugin(name);
    } else {
        unloadPlugin(name);
    }
}

bool PluginManager::hasValidMetadata(QPluginLoader* loader, QString* name)
{
    // find plugin name
    QJsonObject metaData = loader->metaData();
    QJsonValue md2 = metaData.value("MetaData");
    if (!md2.isUndefined() && md2.isObject()) {
        QJsonObject obj = md2.toObject();
        QJsonValue nameVal = obj.value("name");
        if (!nameVal.isUndefined() && nameVal.isString()) {
            QString tmp = nameVal.toString();
            qDebug("Plugin %s found in %s", qPrintable(tmp), qPrintable(loader->fileName()));
            if (name)
                *name = tmp;

            return true;
        }
    }

    return false;
}

void PluginManager::readPlugin(const QString& path)
{
    if (!QLibrary::isLibrary(path))
        return;

    QPluginLoader* loader = new QPluginLoader(path, this);
    QString name;
    if (hasValidMetadata(loader, &name)) {
        m_plugins.append({ name, loader });
    }
}

void PluginManager::loadPlugin(const QString& name)
{
    auto it = std::find_if(m_plugins.cbegin(), m_plugins.cend(), [name](auto p) {
        return p.name == name;
    });

    if (it != m_plugins.end()) {
        Q_ASSERT(it->loader);
        if (it->loader->isLoaded()) {
            qInfo("Plugin %s already loaded", qPrintable(name));
            return;
        }

        QPluginLoader* loader = it->loader;
        MorgothPlugin* instance = qobject_cast<MorgothPlugin*>(loader->instance());
        if (instance == nullptr) {
            qWarning("Plugin \"%s\" invalid; unloading...", qPrintable(name));
            loader->unload();
        } else {
            qInfo("Plugin %s loaded.", qPrintable(name));
            emit pluginStatusChanged(name, true);
        }
    } else {
        qWarning("Plugin %s does not exist", qPrintable(name));
    }
}

void PluginManager::unloadPlugin(const QString& name)
{
    auto it = std::find_if(m_plugins.cbegin(), m_plugins.cend(), [name](auto p) {
        return p.name == name;
    });

    if (it != m_plugins.end()) {
        Q_ASSERT(it->loader);
        if (!it->loader->isLoaded()) {
            qInfo("Plugin %s not loaded", qPrintable(name));
            return;
        }

        if (it->loader->unload()) {
            qInfo("Plugin %s unloaded.", qPrintable(name));
            emit pluginStatusChanged(name, false);
        } else {
            qWarning("Could not unload %s", qPrintable(name));
        }
    }  else {
        qWarning("Plugin %s does not exist", qPrintable(name));
    }
}

} // namespace morgoth
