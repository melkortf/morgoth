#ifndef GAMESERVERMOCK_H
#define GAMESERVERMOCK_H

#include <QtCore/QObject>
#include <QtCore/QtDebug>

class GameServerMock : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.connector.GameServer")

    Q_PROPERTY(QString gameLocation READ gameLocation CONSTANT)

public:
    GameServerMock(QObject* parent = nullptr);

    void connect(const QString& address);
    
    void setGameLocation(const QString& gameLocation) { m_gameLocation = gameLocation; }
    const QString& gameLocation() const;

    QString getConVarValue(const QString& /*conVarName*/) { return QString(); }
    QString getPlayerName(int /*userId*/) { return QString(); }
    quint64 getPlayerSteamId(int /*userId*/) { return 0; }
    void ping() const { };
    void watchConVar(const QString& /*conVarName*/) { }

private:
    QString m_gameLocation;

};

#endif // GAMESERVERMOCK_H
