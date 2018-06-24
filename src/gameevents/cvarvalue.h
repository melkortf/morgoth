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

#ifndef CVARVALUE_H
#define CVARVALUE_H

#include "eventhandler.h"
#include <QtCore/QObject>

namespace morgoth {

class MORGOTH_EXPORT CvarValue: public EventHandler {
    Q_OBJECT

public:
    CvarValue(QString cvar, QObject* parent = nullptr);

    QRegularExpression regex() const override;

    const QString& name() const { return m_name; }
    const QString& value() const { return m_value; }
    const QString& defaultValue() const { return m_defaultValue; }

protected:
    void maybeActivated(const QString& line, const QRegularExpressionMatch& match) override;

private:
    QString m_name; // cvar name, e.g. sv_password
    QString m_value; // cvar value
    QString m_defaultValue;

};

} // namespace morgoth

#endif // CVARVALUE_H
