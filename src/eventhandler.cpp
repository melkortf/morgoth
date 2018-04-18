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

#include "eventhandler.h"

namespace morgoth {

class EventHandlerPrivate {
public:
    QString name;
};

EventHandler::EventHandler(const QString &name, QObject* parent) :
    QObject(parent),
    d(new EventHandlerPrivate{name}) {}

EventHandler::~EventHandler()
{

}

const QString& EventHandler::name() const
{
    return d->name;
}

void EventHandler::maybeActivated(const QString& line, const QRegularExpressionMatch& match)
{
    Q_UNUSED(line);
    Q_UNUSED(match);
    emit activated();
}

} // namespace Morgoth
