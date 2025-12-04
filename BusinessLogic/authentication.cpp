#include "authentication.h"


Qt::ConnectionType Authentication::connect = Qt::AutoConnection;
bool Authentication::userConnect = false;

bool Authentication::createConnect(QString login, QString password)
{
    qDebug() << "createConnect";
    if(login == "мяу" && password == "мяу"){
        return true;
    }else{
        return false;
    }
}

bool Authentication::userAuth()
{
    qDebug() << "userAuth";
    return userConnect;
}

Qt::ConnectionType Authentication::getConnect()
{
    qDebug() << "getConnect";
    return connect;
}

bool Authentication::authentication(QString login, QString password)
{
    qDebug() << "authentication";
    return createConnect(login, password);
}
