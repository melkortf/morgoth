#include <QtCore/QProcess>

class TestingGameServerController {
public:
    TestingGameServerController();
    virtual ~TestingGameServerController();

    void start();
    void connect(const QString& dbusServerAddress);
    void stop();

    void setPath(const QString& path);
    const QString& path() const { return m_path; }

    void setMaxPlayers(int maxPlayers);
    void setAddress(const QString& address);
    void setMap(const QString& map);

private:
    QString m_path;
    QProcess m_process;

};
