#ifndef PRODUCTMANAGER_H
#define PRODUCTMANAGER_H

#include <QString>
#include <QStringList>
#include <QDate>
#include <QByteArray>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>

class ProductManager
{
public:
    static QByteArray encryptData(const QByteArray& data);
    static QByteArray decryptData(const QByteArray& encryptedData);

    static bool insertProduct(
        const QString& name,
        int quantity,
        double price,
        const QDate& delivery_date,
        const QString& supplier,
        const QDate& shipping_date,
        const QString& recipient,
        const QString& about
        );

    static QStringList getAllProducts();

    static bool updateProduct(
        qint64 id,
        const QString& name,
        int quantity,
        double price,
        const QDate& delivery_date,
        const QString& supplier,
        const QDate& shipping_date,
        const QString& recipient,
        const QString& about
        );

    static bool deleteProduct(qint64 id);

private:
    static QSqlDatabase openConnection();
};

#endif // PRODUCTMANAGER_H
