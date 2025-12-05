#include "authentication.h"
#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>

Qt::ConnectionType Authentication::connect = Qt::AutoConnection;
bool Authentication::userConnect = false;

bool Authentication::createConnect(QString login, QString password)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");

    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("myappdb");

    db.setUserName(login);
    db.setPassword(password);

    if (!db.open()) {
        qCritical() << "Ошибка входа или подключения к БД:"
                    << db.lastError().text();
        return false;
    }

    qDebug() << "Аутентификация успешна для пользователя:" << login;

    db.close();

    return true;
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
