#ifndef AUTHENTICATION_H
#define AUTHENTICATION_H

#include <QSqlDatabase>
#include <QString>

class Authentication
{
private:
    static const QString CONNECTION_NAME;
    static bool userConnect;
    static bool createConnect(QString login, QString password);

public:
    static bool authentication(QString login, QString password);
    static bool userAuth();
    static bool unUserAuth();
    static QSqlDatabase getConnect();
};

#endif // AUTHENTICATION_H
