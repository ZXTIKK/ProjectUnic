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
        double price,
        const QString& supplier,
        const QString& about,
        int supplies_quantity,
        const QDate& supplies_date
        );

    static QStringList getAllProducts();

    static bool updateProduct(
        qint64 id,
        const QString& name,
        double price,
        const QString& about
        );

    static bool deleteProduct(qint64 id);

    static bool addShipment(
        qint64 product_id,
        int shipment_quantity,
        const QDate& shipment_date,
        const QString& recipient
        );

    static bool addSupply(
        qint64 product_id,
        int supplies_quantity,
        const QDate& supplies_date,
        const QString& supplier
        );

private:
    static QSqlDatabase openConnection();
};

#endif // PRODUCTMANAGER_H
