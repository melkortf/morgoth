#include "updatenotificationevent.h"

namespace morgoth {

UpdateNotificationEvent::UpdateNotificationEvent(QObject *parent) :
    EventHandler(Name, parent) {}

QRegularExpression UpdateNotificationEvent::regex() const
{
    return QRegularExpression(".*MasterRequestRestart$");
}

} // namespace morgoth
