#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <QApplication>
#include <QString>
#include <QDebug>

class Authentication
{


private:
    static Qt::ConnectionType connect;
    static bool userConnect;
    static bool createConnect(QString login, QString password);

public:
    static bool userAuth();
    static Qt::ConnectionType getConnect();
    static bool authentication(QString, QString);
};

#endif // AUTHENTICATION_H
