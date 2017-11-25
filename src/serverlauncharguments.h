#ifndef SERVERLAUNCHARGUMENTS_H
#define SERVERLAUNCHARGUMENTS_H

#include <QSharedData>
#include <QString>

namespace morgoth {

class ServerLaunchArgumentsData;

class ServerLaunchArguments {
public:
    ServerLaunchArguments();
    virtual ~ServerLaunchArguments();

    unsigned port() const;
    void setPort(unsigned port);
    bool isSecured() const;
    void setSecured(bool secured);
    QString initialMap() const;
    void setInitialMap(const QString& initialMap);

    QString asSrcdsArguments() const;

private:
    QSharedDataPointer<ServerLaunchArgumentsData> d;

};

} // namespace morgoth

#endif // SERVERLAUNCHARGUMENTS_H
