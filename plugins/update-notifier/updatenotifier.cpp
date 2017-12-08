#include "updatenotifier.h"
#include <QtCore>

namespace morgoth {

UpdateNotifier::UpdateNotifier(QObject *parent) : QObject(parent)
{
    qDebug() << "UpdateNotifier loaded";
}

UpdateNotifier::~UpdateNotifier()
{
    qDebug() << "UpdateNotifier unloaded";
}

} // namespace morgoth
