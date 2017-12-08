#ifndef UPDATENOTIFIER_H
#define UPDATENOTIFIER_H

#include "morgothplugin.h"
#include <QtCore/QObject>

namespace morgoth {

class UpdateNotifier : public QObject, public MorgothPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.morgoth.MorgothPlugin")
    Q_INTERFACES(MorgothPlugin)

public:
    explicit UpdateNotifier(QObject *parent = nullptr);

    virtual ~UpdateNotifier();

};

} // namespace morgoth

#endif // UPDATENOTIFIER_H
