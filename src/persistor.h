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

#ifndef PERSISTOR_H
#define PERSISTOR_H

#include "morgoth_export.h"
#include "pluginmanager.h"
#include "servermanager.h"
#include <QtSql/QSqlDatabase>

namespace morgoth {

class PersistorPrivate;

/**
 * \brief The Persistor class is responsible for storing all servers and their
 *  configs.
 */
class MORGOTH_EXPORT Persistor : public QObject {
    Q_OBJECT

signals:
    /**
     * \brief Emitted when the persistor has been initialized.
     */
    void initialized();

public:
    /**
     * Creates a new \c Persistor instance for the given \c serverManager and
     * \c pluginManager.
     */
    explicit Persistor(ServerManager* serverManager, PluginManager* pluginManager,
                       QObject* parent = nullptr);

    /**
     * Destroys this \c Persistor instance.
     */
    virtual ~Persistor();

private:
    void initializeDatabase();
    void restoreServers();
    void restorePlugins();

private slots:
    void initialize();
    void storeServer(Server* server);
    void deleteServer(Server* server);
    void storeConfigurationEntry(const QString& key, const QString& value);
    void storePluginState(const QString& pluginName, bool enabled);

private:
    QScopedPointer<PersistorPrivate> d;

};

} // namespace morgoth

#endif // PERSISTOR_H
