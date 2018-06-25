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

#ifndef STATUSHOSTNAME_H
#define STATUSHOSTNAME_H

#include "gameevent.h"
#include <QtCore/QObject>

namespace morgoth {

class MORGOTH_EXPORT StatusHostname : public GameEvent {
    Q_OBJECT

public:
    StatusHostname(QObject* parent = nullptr);

    QRegularExpression regex() const override;

    const QString& hostname() const { return m_hostname; }

    static auto constexpr Name = "status.line.hostname";

protected:
    void maybeActivated(const QString& line, const QRegularExpressionMatch& match) override;

private:
    QString m_hostname;

};

} // namespace morgoth

#endif // STATUSHOSTNAME_H
