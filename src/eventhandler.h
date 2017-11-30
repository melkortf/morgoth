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

#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "morgoth_export.h"
#include <QtCore/QObject>
#include <QtCore/QRegularExpression>

namespace morgoth {

class LogListener;

/**
 * \brief The EventHandler class captures interesting lines in the running
 *  server's log.
 * \sa ServerCoordinator.
 */
class MORGOTH_EXPORT EventHandler : public QObject {
    Q_OBJECT

signals:
    /**
     * \brief Emitted right after the event happened.
     */
    void activated();

public:
    /**
     * \brief Creates a new \c EventHandler instance.
     * \param name The name of the event.
     * \param parent Passed to QObject.
     */
    explicit EventHandler(const QString& name, QObject* parent = nullptr);

    /**
     * \brief Returns the regular expression that marks the occurrence of
     *  this event.
     */
    virtual QRegularExpression regex() const = 0;

    /**
     * \brief Name of this event.
     */
    const QString& name() const { return m_name; }

protected:
    virtual void maybeActivated(const QString& line, const QRegularExpressionMatch& match);

private:
    friend class LogListener;
    QString m_name;

};

} // namespace Morgoth

#endif // EVENTHANDLER_H
