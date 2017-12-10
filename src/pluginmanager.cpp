#include "pluginmanager.h"
#include <QtCore>

namespace morgoth {

PluginManager::PluginManager(QObject *parent) :
    QObject(parent)
{

}

void PluginManager::addPluginsDir(const QString& path)
{
    QDirIterator it(path, QDir::Files | QDir::NoDotAndDotDot | QDir::Readable, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        readPlugin(it.next());
    }
}

void PluginManager::readPlugin(const QString& path)
{
    if (!QLibrary::isLibrary(path))
        return;

    QPluginLoader loader(path);
    QJsonObject metaData = loader.metaData();

    // find plugin name
    QJsonValue md2 = metaData.value("MetaData");
    if (!md2.isUndefined() && md2.isObject()) {
        QJsonObject obj = md2.toObject();
        QJsonValue nameVal = obj.value("name");
        if (!nameVal.isUndefined() && nameVal.isString()) {
            QString name = nameVal.toString();
            qDebug("Plugin %s found in %s", qPrintable(name), qPrintable(path));

            MorgothPlugin* instance = qobject_cast<MorgothPlugin*>(loader.instance());
            m_plugins.append({ name, path, metaData, instance });
        }
    }
}

} // namespace morgoth
