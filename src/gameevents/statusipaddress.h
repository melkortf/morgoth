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

#ifndef STATUSIPADDRESS_H
#define STATUSIPADDRESS_H

#include "gameevent.h"
#include <QtCore/QObject>

namespace morgoth {

class MORGOTH_EXPORT StatusIpAddress : public GameEvent {
    Q_OBJECT

public:
    explicit StatusIpAddress(QObject* parent = nullptr);

    QRegularExpression regex() const override;

    const QString& ip() const { return m_ip; }
    quint32 port() const { return m_port; }

    static auto constexpr Name = "status.line.ip";

protected:
    void maybeActivated(const QString& line, const QRegularExpressionMatch& match) override;

private:
    QString m_ip;
    quint32 m_port;

};

} // namespace morgoth

#endif // STATUSIPADDRESS_H
