#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QDate>
#include <QDebug>
#include <QSqlError>
#include <QByteArray>

class ProductManager
{
private:
    QSqlDatabase openConnection();
    QByteArray generateKey(const QString& username, const QString& pasord);
    QByteArray encryptData(const QByteArray& data, const QByteArray& key);
    QByteArray decryptData(const QByteArray& enecrypteData, const QByteArray& key);

public:
    ProductManager();

    bool insertProduct(
        const QString& name,
        int quantity,
        double price,
        const QDate& delivery_date,
        const QString& supplier,
        const QDate& shipping_date,
        const QString& recipient
        );

    bool updateProduct(
        qint64 id,
        const QString& name,
        int quantity,
        double price,
        const QDate& delivery_date,
        const QString& supplier,
        const QDate& shipping_date,
        const QString& recipient
        );

    bool deleteProduct(qint64 id);

    QStringList getAllProducts();
};

#endif // PRODUCTMANAGER_H
