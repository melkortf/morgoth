#include "pluginmanager.h"
#include "morgothdaemon.h"
#include "pluginmanageradaptor.h"
#include <QtCore>

namespace morgoth {

class PluginManagerPrivate {
public:
    explicit PluginManagerPrivate(PluginManager* d);

    bool hasValidMetadata(QPluginLoader* loader, QString* name);
    void readPlugin(const QString& path);
    void loadPlugin(const QString& name);
    void unloadPlugin(const QString& name);

    struct PluginData {
        QString name;
        QPluginLoader* loader;
    };

    PluginManager* d;
    QList<PluginData> plugins;
};

PluginManagerPrivate::PluginManagerPrivate(PluginManager *d) :
    d(d)
{

}

bool PluginManagerPrivate::hasValidMetadata(QPluginLoader *loader, QString *name)
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

void PluginManagerPrivate::readPlugin(const QString &path)
{
    if (!QLibrary::isLibrary(path))
        return;

    QPluginLoader* loader = new QPluginLoader(path, d);
    QString name;
    if (hasValidMetadata(loader, &name)) {
        plugins.append({ name, loader });
    }
}

void PluginManagerPrivate::loadPlugin(const QString &name)
{
    auto it = std::find_if(plugins.cbegin(), plugins.cend(), [name](auto p) {
        return p.name == name;
    });

    if (it != plugins.end()) {
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
            emit d->pluginStatusChanged(name, true);
        }
    } else {
        qWarning("Plugin %s does not exist", qPrintable(name));
    }
}

void PluginManagerPrivate::unloadPlugin(const QString &name)
{
    auto it = std::find_if(plugins.cbegin(), plugins.cend(), [name](auto p) {
        return p.name == name;
    });

    if (it != plugins.end()) {
        Q_ASSERT(it->loader);
        if (!it->loader->isLoaded()) {
            qInfo("Plugin %s not loaded", qPrintable(name));
            return;
        }

        if (it->loader->unload()) {
            qInfo("Plugin %s unloaded.", qPrintable(name));
            emit d->pluginStatusChanged(name, false);
        } else {
            qWarning("Could not unload %s", qPrintable(name));
        }
    }  else {
        qWarning("Plugin %s does not exist", qPrintable(name));
    }
}

PluginManager::PluginManager(QObject *parent) :
    QObject(parent),
    d(new PluginManagerPrivate(this))
{
    new PluginManagerAdaptor(this);
    morgothd->dbusConnection().registerObject("/plugins", this);
}

PluginManager::~PluginManager()
{

}

QStringList PluginManager::availablePlugins() const
{
    QStringList res;
    for (const auto& p: d->plugins) {
        res << p.name;
    }

    return res;
}

void PluginManager::addPluginsDir(const QString& path)
{
    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        d->readPlugin(it.next());
    }
}

void PluginManager::setPluginStatus(const QString& name, bool enabled)
{
    if (enabled) {
        d->loadPlugin(name);
    } else {
        d->unloadPlugin(name);
    }
}

} // namespace morgoth
