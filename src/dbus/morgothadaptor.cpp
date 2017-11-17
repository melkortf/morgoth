#include "morgothadaptor.h"

namespace morgoth { namespace dbus {

MorgothAdaptor::MorgothAdaptor(QCoreApplication* app) :
    QDBusAbstractAdaptor(app),
    m_app(app)
{

}

QString MorgothAdaptor::version() const
{
    return m_app->applicationVersion();
}

void MorgothAdaptor::quit()
{
    m_app->quit();
}

}} // namespace morgoth::dbus
