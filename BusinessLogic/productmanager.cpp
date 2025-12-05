#include "ProductManager.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>
#include <QCryptographicHash>

const QString DB_HOST = "localhost";
const int DB_PORT = 5432;
const QString DB_NAME = "myappdb";
const QString TECH_USER = "postgres";
const QString TECH_PASS = "Zxc123!!!";

QSqlDatabase ProductManager::openConnection()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName(DB_HOST);
    db.setPort(DB_PORT);
    db.setDatabaseName(DB_NAME);
    db.setUserName(TECH_USER);
    db.setPassword(TECH_PASS);

    if (!db.open()) {
        qCritical() << "❌ Ошибка подключения к БД:" << db.lastError().text();
    }
    return db;
}

QByteArray ProductManager::generateKey(const QString& username, const QString& password)
{
    QString combined = username + password;
    return QCryptographicHash::hash(
        combined.toUtf8(),
        QCryptographicHash::Sha256
        );
}

QByteArray ProductManager::encryptData(const QByteArray& data, const QByteArray& key)
{
    return data.toBase64();
}

QByteArray ProductManager::decryptData(const QByteArray& encryptedData, const QByteArray& key)
{
    return QByteArray::fromBase64(encryptedData);
}

ProductManager::ProductManager() {}
bool ProductManager::insertProduct(
    const QString& name,
    int quantity,
    double price,
    const QDate& delivery_date,
    const QString& supplier,
    const QDate& shipping_date,
    const QString& recipient)
{
    QByteArray encryptionKey = generateKey(TECH_USER, TECH_PASS);

    QByteArray name_enc = encryptData(name.toUtf8(), encryptionKey);
    QByteArray quantity_enc = encryptData(QByteArray::number(quantity), encryptionKey);
    QByteArray price_enc = encryptData(QByteArray::number(price, 'f', 2), encryptionKey);
    QByteArray delivery_date_enc = encryptData(delivery_date.toString("yyyy-MM-dd").toUtf8(), encryptionKey);
    QByteArray supplier_enc = encryptData(supplier.toUtf8(), encryptionKey);
    QByteArray shipping_date_enc = encryptData(shipping_date.toString("yyyy-MM-dd").toUtf8(), encryptionKey);
    QByteArray recipient_enc = encryptData(recipient.toUtf8(), encryptionKey);

    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("INSERT INTO products ("
                  "\"product name\", quantity, price, \"delivery date\", supplier, \"shipping date\", recipient) "
                  "VALUES (?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(name_enc);
    query.addBindValue(quantity_enc);
    query.addBindValue(price_enc);
    query.addBindValue(delivery_date_enc);
    query.addBindValue(supplier_enc);
    query.addBindValue(shipping_date_enc);
    query.addBindValue(recipient_enc);

    if (!query.exec()) {
        qCritical() << "❌ Error INSERT:" << query.lastError().text();
        db.close();
        return false;
    }

    db.close();
    qDebug() << "✅ Product added (encrypted):" << name;
    return true;
}
QStringList ProductManager::getAllProducts()
{
    QStringList resultList;
    QByteArray encryptionKey = generateKey(TECH_USER, TECH_PASS);

    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return resultList;

    QSqlQuery query(db);
    if (!query.exec("SELECT id, \"product name\", quantity, price, \"delivery date\", "
                    "supplier, \"shipping date\", recipient FROM products"))
    {
        qCritical() << "❌ Ошибка SELECT:" << query.lastError().text();
        db.close();
        return resultList;
    }

    while (query.next()) {
        QString row;

        row += QString("ID: %1 | ").arg(query.value(0).toLongLong());

        for (int i = 1; i < 8; ++i) {
            QByteArray encryptedData = query.value(i).toByteArray();
            QByteArray decryptedBytes = decryptData(encryptedData, encryptionKey);

            QString decryptedString = QString::fromUtf8(decryptedBytes);

            row += QString("%1 | ").arg(decryptedString);
        }
        resultList.append(row.trimmed());
    }

    db.close();
    qDebug() << "✅ Получено" << resultList.size() << "записей (дешифровано).";
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
    const QString& recipient)
{
    QByteArray encryptionKey = generateKey(TECH_USER, TECH_PASS);

    // Шифруем все обновляемые данные
    QByteArray name_enc = encryptData(name.toUtf8(), encryptionKey);
    QByteArray quantity_enc = encryptData(QByteArray::number(quantity), encryptionKey);
    QByteArray price_enc = encryptData(QByteArray::number(price, 'f', 2), encryptionKey);
    QByteArray delivery_date_enc = encryptData(delivery_date.toString("yyyy-MM-dd").toUtf8(), encryptionKey);
    QByteArray supplier_enc = encryptData(supplier.toUtf8(), encryptionKey);
    QByteArray shipping_date_enc = encryptData(shipping_date.toString("yyyy-MM-dd").toUtf8(), encryptionKey);
    QByteArray recipient_enc = encryptData(recipient.toUtf8(), encryptionKey);

    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("UPDATE products SET "
                  "\"product name\" = ?, quantity = ?, price = ?, \"delivery date\" = ?, "
                  "supplier = ?, \"shipping date\" = ?, recipient = ? "
                  "WHERE id = ?"); // Условие WHERE для выбора записи

    // Привязываем зашифрованные значения
    query.addBindValue(name_enc);
    query.addBindValue(quantity_enc);
    query.addBindValue(price_enc);
    query.addBindValue(delivery_date_enc);
    query.addBindValue(supplier_enc);
    query.addBindValue(shipping_date_enc);
    query.addBindValue(recipient_enc);

    // Привязываем ID (не зашифрован)
    query.addBindValue(id);

    if (!query.exec()) {
        qCritical() << "Error UPDATE:" << query.lastError().text();
        db.close();
        return false;
    }

    db.close();
    qDebug() << "Product ID" << id << " updated (encrypted).";
    return true;
}
bool ProductManager::deleteProduct(qint64 id)
{
    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return false;

    QSqlQuery query(db);
    query.prepare("DELETE FROM products WHERE id = ?");

    // Привязываем ID
    query.addBindValue(id);

    if (!query.exec()) {
        qCritical() << "Error DELETE:" << query.lastError().text();
        db.close();
        return false;
    }

    db.close();
    qDebug() << "Product ID" << id << " deleted.";
    return true;
}
