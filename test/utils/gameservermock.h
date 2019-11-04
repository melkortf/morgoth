#ifndef GAMESERVERMOCK_H
#define GAMESERVERMOCK_H

#include <QtCore/QObject>
#include <QtCore/QtDebug>

class GameServerMock : public QObject {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.connector.GameServer")

    Q_PROPERTY(QString gameLocation READ gameLocation CONSTANT)
    Q_PROPERTY(QString map READ map NOTIFY mapChanged)
    Q_PROPERTY(int maxPlayers READ maxPlayers)
    Q_PROPERTY(QString address READ address)

signals:
    void mapChanged(QString map);

public:
    GameServerMock(QObject* parent = nullptr);

    void connect(const QString& address);
    
    void setGameLocation(const QString& gameLocation) { m_gameLocation = gameLocation; }
    QString gameLocation() const { return m_gameLocation; }

    void setMap(const QString& map);
    QString map() const { return m_map; }

    void setMaxPlayers(int maxPlayers);
    int maxPlayers() const { return m_maxPlayers; }

    void setAddress(const QString& address);
    QString address() const { return m_address; }

    Q_INVOKABLE QString getConVarValue(const QString& /*conVarName*/) { return QString(); }
    QString getPlayerName(int /*userId*/) { return QString(); }
    quint64 getPlayerSteamId(int /*userId*/) { return 0; }
    void ping() const { };
    void watchConVar(const QString& /*conVarName*/) { }

private:
    QString m_gameLocation;
    QString m_map;
    int m_maxPlayers;
    QString m_address;

};

#endif // GAMESERVERMOCK_H
