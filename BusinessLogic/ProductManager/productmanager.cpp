#include "ProductManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QDate>
#include "../Authentication/authentication.h"

QSqlDatabase ProductManager::openConnection()
{
    QSqlDatabase db = Authentication::getConnect();

    if (!db.open()) {
        qCritical() << "Error connect of bd:" << db.lastError().text();
    }
    return db;
}

QByteArray ProductManager::encryptData(const QByteArray& data)
{
    return data.toBase64();
}

QByteArray ProductManager::decryptData(const QByteArray& encryptedData)
{
    return QByteArray::fromBase64(encryptedData);
}

bool ProductManager::insertProduct(
    const QString& name,
    int quantity,
    double price,
    const QDate& delivery_date,
    const QString& supplier,
    const QDate& shipping_date,
    const QString& recipient,
    const QString& about)
{
    QByteArray name_enc = encryptData(name.toUtf8());
    QByteArray quantity_enc = encryptData(QByteArray::number(quantity));
    QByteArray price_enc = encryptData(QByteArray::number(price, 'f', 2));
    QByteArray delivery_date_enc = encryptData(delivery_date.toString("yyyy-MM-dd").toUtf8());
    QByteArray supplier_enc = encryptData(supplier.toUtf8());
    QByteArray shipping_date_enc = encryptData(shipping_date.toString("yyyy-MM-dd").toUtf8());
    QByteArray recipient_enc = encryptData(recipient.toUtf8());
    QByteArray about_enc = encryptData(about.toUtf8());

    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("INSERT INTO products ("
                    "product_name, quantity, price, delivery_date, supplier, shipping_date, recipient, about"
                    ") VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(name_enc);
    query.addBindValue(quantity_enc);
    query.addBindValue(price_enc);
    query.addBindValue(delivery_date_enc);
    query.addBindValue(supplier_enc);
    query.addBindValue(shipping_date_enc);
    query.addBindValue(recipient_enc);
    query.addBindValue(about_enc);

    if (!query.exec()) {
        qCritical() << "Error INSERT:" << query.lastError().text();
        return false;
    }
    qDebug() << "Product added (Base64 encoded):" << name;
    return true;
}

QStringList ProductManager::getAllProducts()
{
    QStringList resultList;

    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return resultList;

    QSqlQuery query(db);
    if (!query.exec("SELECT id, product_name, quantity, price, delivery_date, supplier, shipping_date, recipient, about FROM products"))
    {
        qCritical() << "Error SELECT:" << query.lastError().text();
        return resultList;
    }

    while (query.next()) {
        QString row;

        row += QString("ID: %1 | ").arg(query.value(0).toLongLong());

        for (int i = 1; i < 9; ++i) {
            QByteArray encryptedData = query.value(i).toByteArray();
            QByteArray decryptedBytes = decryptData(encryptedData);

            QString decryptedString = QString::fromUtf8(decryptedBytes);

            row += QString("%1 | ").arg(decryptedString);
        }
        resultList.append(row.trimmed());
    }

    qDebug() << "Получено" << resultList.size() << "записей (Base64 decoded).";
    return resultList;
}

bool ProductManager::updateProduct(
    qint64 id,
    const QString& name,
    int quantity,
    double price,
    const QDate& delivery_date,
    const QString& supplier,
    const QDate& shipping_date,
    const QString& recipient,
    const QString& about)
{

    QByteArray name_enc = encryptData(name.toUtf8());
    QByteArray quantity_enc = encryptData(QByteArray::number(quantity));
    QByteArray price_enc = encryptData(QByteArray::number(price, 'f', 2));
    QByteArray delivery_date_enc = encryptData(delivery_date.toString("yyyy-MM-dd").toUtf8());
    QByteArray supplier_enc = encryptData(supplier.toUtf8());
    QByteArray shipping_date_enc = encryptData(shipping_date.toString("yyyy-MM-dd").toUtf8());
    QByteArray recipient_enc = encryptData(recipient.toUtf8());
    QByteArray about_enc = encryptData(about.toUtf8());

    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("UPDATE products SET "
                  "product_name = ?, quantity = ?, price = ?, delivery_date = ?, supplier = ?, shipping_date = ?, recipient = ?, about = ?"
                  "product_name, quantity, price, delivery_date, supplier, shipping_date, recipient, about"
                  "WHERE id = ?");

    query.addBindValue(name_enc);
    query.addBindValue(quantity_enc);
    query.addBindValue(price_enc);
    query.addBindValue(delivery_date_enc);
    query.addBindValue(supplier_enc);
    query.addBindValue(shipping_date_enc);
    query.addBindValue(recipient_enc);
    query.addBindValue(about_enc);

    query.addBindValue(id);

    if (!query.exec()) {
        qCritical() << "Error UPDATE:" << query.lastError().text();
        return false;
    }

    qDebug() << "Product ID" << id << " updated (Base64 encoded).";
    return true;
}

bool ProductManager::deleteProduct(qint64 id)
{
    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("DELETE FROM products WHERE id = ?");

    query.addBindValue(id);

    if (!query.exec()) {
        qCritical() << "Error DELETE:" << query.lastError().text();
        return false;
    }

    qDebug() << "Product ID" << id << " deleted.";
    return true;
}
