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

#ifndef TMUXSESSIONWRAPPER_H
#define TMUXSESSIONWRAPPER_H

#include "morgoth_export.h"
#include <QtCore/QSharedPointer>
#include <QtCore/QString>

namespace morgoth {

class TmuxSessionWrapperPrivate;

/**
 * \brief The TmuxSessionWrapper class wraps all tmux-oriented tasks morgoth
 *  needs.
 */
class MORGOTH_EXPORT TmuxSessionWrapper {
public:
    /**
     * \brief Creates a new TmuxSessionWrapper instance.
     */
    TmuxSessionWrapper();

    /**
     * \brief Kills the session if it was not destroyed earlier.
     */
    virtual ~TmuxSessionWrapper();

    /**
     * \brief Creates a new tmux session.
     * \return \c true if the session was created properly, \c false otherwise.
     */
    bool create();

    /**
     * \brief Pipes the terminal output to \c dest.
     * \return \c false if the session was not opened prior to calling this method.
     */
    bool redirectOutput(const QString& dest);

    /**
     * \brief Executes tmux's \c send-keys command.
     * \param keys Command to be sent to this tmux session.
     * \return \c false if the session was not opened prior to calling this method.
     */
    bool sendKeys(const QString& keys);

    /**
     * \brief Kills this tmux session.
     * \return \c true if the session was successfully killed.
     */
    bool kill();

    /**
     * \brief The randomly generated name of this tmux session.
     */
    const QString& name() const;

    /**
     * \brief Queries tmux to find out whether this session is running or not.
     */
    bool exists() const;

    const QString user() const;
    void setUser(const QString& user);

    /**
     * \brief Returns \c true if this session exists.
     */
    operator bool() const { return exists(); }

private:
    QScopedPointer<TmuxSessionWrapperPrivate> d;

};

} // namespace Morgoth

#endif // TMUXSESSIONWRAPPER_H
