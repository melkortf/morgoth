#ifndef MORGOTHADAPTOR_H
#define MORGOTHADAPTOR_H

#include <QtCore/QCoreApplication>
#include <QtDBus/QDBusAbstractAdaptor>

namespace morgoth { namespace dbus {

/**
 * \brief The MorgothAdaptor class is a D-Bus adaptor for the morgoth
 * daemon.
 */
class MorgothAdaptor : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.morgoth.Morgoth")
    Q_PROPERTY(QString version READ version)

public:
    explicit MorgothAdaptor(QCoreApplication* app);

    QString version() const;

public slots:
    Q_NOREPLY void quit();

private:
    QCoreApplication* m_app;

};

}} // namespace morgoth::dbus

#endif // MORGOTHADAPTOR_H
