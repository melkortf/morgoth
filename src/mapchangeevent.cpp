#include "mapchangeevent.h"

namespace Morgoth {

MapChangeEvent::MapChangeEvent(QObject *parent) :
    EventHandler(Name, parent) {}

QRegularExpression MapChangeEvent::regex() const
{
    return QRegularExpression(".*Host_Changelevel.*");
}

} // namespace Morgoth
