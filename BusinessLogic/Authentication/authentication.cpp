#include "authentication.h"
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QString>
#include <QCryptographicHash>


const QString Authentication::CONNECTION_NAME = "Connection";
bool Authentication::userConnect = false;

bool Authentication::createConnect(QString login, QString password)
{

    if (QSqlDatabase::contains(CONNECTION_NAME)) {

        QSqlDatabase oldDb = QSqlDatabase::database(CONNECTION_NAME);
        if (oldDb.isOpen()) {
            oldDb.close();
        }
        QSqlDatabase::removeDatabase(CONNECTION_NAME);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", CONNECTION_NAME);

    db.setHostName("localhost");
    db.setPort(5432);
    db.setDatabaseName("myappdb");

    db.setUserName(login);
    db.setPassword(password);

    if (!db.open()) {
        qCritical() << "Ошибка входа или подключения к БД:"
                    << db.lastError().text();

        Authentication::userConnect = false;

        QSqlDatabase::removeDatabase(CONNECTION_NAME);
        return false;
    }

    qDebug() << "Аутентификация успешна для пользователя:" << login;

    Authentication::userConnect = true;

    return true;
}

bool Authentication::userAuth()
{
    qDebug() << "userAuth: Проверка статуса аутентификации";
    return userConnect;
}

QSqlDatabase Authentication::getConnect()
{
    qDebug() << "getConnect: Возврат активного соединения";
    return QSqlDatabase::database(CONNECTION_NAME);
}

bool Authentication::authentication(QString login, QString password)
{
    qDebug() << "authentication: Попытка аутентификации";
    return createConnect(login, password);
}

