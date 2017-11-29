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

#include "morgothadaptor.h"

namespace morgoth { namespace dbus {

MorgothAdaptor::MorgothAdaptor(MorgothDaemon* daemon) :
    QDBusAbstractAdaptor(daemon),
    m_daemon(daemon)
{

}

QString MorgothAdaptor::name() const
{
    return m_daemon->applicationName();
}

QString MorgothAdaptor::version() const
{
    return m_daemon->applicationVersion();
}

void MorgothAdaptor::quit()
{
    m_daemon->quit();
}

}} // namespace morgoth::dbus
