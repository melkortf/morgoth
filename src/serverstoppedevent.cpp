#include "serverstoppedevent.h"

namespace Morgoth {

ServerStoppedEvent::ServerStoppedEvent(QObject *parent) :
    EventHandler(Name, parent)
{

}

QRegularExpression ServerStoppedEvent::regex() const
{
    return QRegularExpression("Server Quit$");
}

} // namespace Morgoth
