#ifndef TMUXSESSIONWRAPPER_H
#define TMUXSESSIONWRAPPER_H

#include <QtCore/QString>

namespace morgoth {

class TmuxSessionWrapper {
public:
    TmuxSessionWrapper();
    virtual ~TmuxSessionWrapper();

    bool create();
    bool redirectOutput(const QString& dest);
    bool sendKeys(const QString& keys);
    bool kill();

    const QString& name() const { return m_name; }

private:
    void findTmuxExec();
    void generateRandomName();

    QString m_tmuxExec;
    QString m_name;
    bool m_open = false;

};

} // namespace Morgoth

#endif // TMUXSESSIONWRAPPER_H
