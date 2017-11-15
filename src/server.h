#ifndef SERVER_H
#define SERVER_H

#include <QtCore/QObject>
#include <QtCore/QString>

namespace Morgoth {

class ServerCoordinator;

class Server : public QObject {
    Q_OBJECT

public:
    Server(const QString& path, const QString& name, QObject* parent = nullptr);
    virtual ~Server();

    const QString& name() const { return m_name; }
    const QString& path() const { return m_path; }
    bool isValid() const { return m_valid; }
    const QString& srcdsExec() const { return m_srcdsExec; }
    ServerCoordinator* coordinator() { return m_coordinator; }

private:
    void discover();

    QString m_name;
    QString m_path;
    bool m_valid;
    QString m_srcdsExec;
    ServerCoordinator* m_coordinator;

};

} // namespace Morgoth

#endif // SERVER_H
