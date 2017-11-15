#ifndef MAPCHANGEEVENT_H
#define MAPCHANGEEVENT_H

#include "eventhandler.h"

namespace Morgoth {

class MapChangeEvent : public EventHandler {
    Q_OBJECT

public:
    explicit MapChangeEvent(QObject *parent = nullptr);

    QRegularExpression regex() const override;

    static auto constexpr Name = "mapchange";

};

} // namespace Morgoth

#endif // MAPCHANGEEVENT_H
