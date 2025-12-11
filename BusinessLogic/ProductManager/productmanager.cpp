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
    double price,
    const QString& supplier,
    const QString& about,
    int supplies_quantity,
    const QDate& supplies_date)
{
    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return false;

    if (!db.transaction()) {
        qCritical() << "Failed to start transaction for product insert:" << db.lastError().text();
        return false;
    }

    QByteArray name_enc = encryptData(name.toUtf8());
    QByteArray initial_quantity_enc = encryptData(QByteArray::number(supplies_quantity));
    QByteArray price_enc = encryptData(QByteArray::number(price, 'f', 2));
    QByteArray about_enc = encryptData(about.toUtf8());

    QSqlQuery query(db);
    query.prepare("INSERT INTO products ("
                  "product_name, quantity, price, about"
                  ") VALUES (?, ?, ?, ?) RETURNING id");

    query.addBindValue(name_enc);
    query.addBindValue(initial_quantity_enc);
    query.addBindValue(price_enc);
    query.addBindValue(about_enc);

    if (!query.exec()) {
        qCritical() << "Error INSERT into products:" << query.lastError().text();
        db.rollback();
        return false;
    }

    if (!query.next()) {
        qCritical() << "Error: Could not retrieve last inserted product ID.";
        db.rollback();
        return false;
    }
    qint64 product_id = query.value(0).toLongLong();

    QByteArray supplies_quantity_enc = encryptData(QByteArray::number(supplies_quantity));
    QByteArray supplies_date_enc = encryptData(supplies_date.toString("yyyy-MM-dd").toUtf8());
    QByteArray supplier_enc = encryptData(supplier.toUtf8());

    QSqlQuery supplies_query(db);
    supplies_query.prepare("INSERT INTO supplies (supplies_date, quantity, id_product, supplier_name) "
                           "VALUES (?, ?, ?, ?)");

    supplies_query.addBindValue(supplies_date_enc);
    supplies_query.addBindValue(supplies_quantity_enc);
    supplies_query.addBindValue(product_id);
    supplies_query.addBindValue(supplier_enc);

    if (!supplies_query.exec()) {
        qCritical() << "Error INSERT into supplies (product ID:" << product_id << "):" << supplies_query.lastError().text();
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        qCritical() << "Failed to commit transaction for product insert:" << db.lastError().text();
        return false;
    }

    qDebug() << "Product and initial Supply added with ID:" << product_id;
    return true;
}

QStringList ProductManager::getAllProducts()
{
    QStringList resultList;

    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return resultList;

    QSqlQuery query(db);

    const QString selectQuery =
        "SELECT p.id, p.product_name, p.quantity AS current_quantity, p.price, p.about, "
        "s.shipment_date, s.quantity AS shipment_quantity, s.recipient_name, "
        "su.supplies_date, su.quantity AS supplies_quantity, su.supplier_name "
        "FROM products p "
        "LEFT JOIN shipment s ON p.id = s.id_product "
        "LEFT JOIN supplies su ON p.id = su.id_product";


    if (!query.exec(selectQuery))
    {
        qCritical() << "Error SELECT with JOINS:" << query.lastError().text();
        return resultList;
    }

    while (query.next()) {
        QString row;
        row += QString("ID: %1 | ").arg(query.value(0).toLongLong());

        for (int i = 1; i < 11; ++i) {
            QVariant value = query.value(i);
            QString decryptedString;

            if (value.isNull() || value.toString().isEmpty()) {
                decryptedString = "N/A";
            } else {
                QByteArray encryptedData = value.toByteArray();
                QByteArray decryptedBytes = decryptData(encryptedData);
                decryptedString = QString::fromUtf8(decryptedBytes);
            }
            row += QString("%1 | ").arg(decryptedString);
        }
        resultList.append(row.trimmed());
    }

    qDebug() << "Получено" << resultList.size() << "записей с полной информацией (Base64 decoded).";
    return resultList;
}

bool ProductManager::updateProduct(
    qint64 id,
    const QString& name,
    double price,
    const QString& about)
{
    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return false;

    QByteArray name_enc = encryptData(name.toUtf8());
    QByteArray price_enc = encryptData(QByteArray::number(price, 'f', 2));
    QByteArray about_enc = encryptData(about.toUtf8());

    QSqlQuery product_query(db);

    product_query.prepare("UPDATE products SET "
                          "product_name = ?, price = ?, about = ? "
                          "WHERE id = ?");

    product_query.addBindValue(name_enc);
    product_query.addBindValue(price_enc);
    product_query.addBindValue(about_enc);
    product_query.addBindValue(id);

    if (!product_query.exec()) {
        qCritical() << "Error UPDATE products (ID:" << id << "):" << product_query.lastError().text();
        return false;
    }

    qDebug() << "Product ID" << id << " updated (Base64 encoded).";
    return true;
}

bool ProductManager::deleteProduct(qint64 id)
{
    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return false;

    if (!db.transaction()) {
        qCritical() << "Failed to start transaction for delete:" << db.lastError().text();
        return false;
    }

    QSqlQuery shipment_query(db);
    shipment_query.prepare("DELETE FROM shipment WHERE id_product = ?");
    shipment_query.addBindValue(id);
    if (!shipment_query.exec()) {
        qCritical() << "Error DELETE from shipment (ID:" << id << "):" << shipment_query.lastError().text();
        db.rollback();
        return false;
    }

    QSqlQuery supplies_query(db);
    supplies_query.prepare("DELETE FROM supplies WHERE id_product = ?");
    supplies_query.addBindValue(id);
    if (!supplies_query.exec()) {
        qCritical() << "Error DELETE from supplies (ID:" << id << "):" << supplies_query.lastError().text();
        db.rollback();
        return false;
    }

    QSqlQuery product_query(db);
    product_query.prepare("DELETE FROM products WHERE id = ?");
    product_query.addBindValue(id);
    if (!product_query.exec()) {
        qCritical() << "Error DELETE from products (ID:" << id << "):" << product_query.lastError().text();
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        qCritical() << "Failed to commit transaction for delete:" << db.lastError().text();
        return false;
    }

    qDebug() << "Product and related records ID" << id << " deleted.";
    return true;
}

bool ProductManager::addShipment(
    qint64 product_id,
    int shipment_quantity,
    const QDate& shipment_date,
    const QString& recipient)
{
    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return false;

    if (!db.transaction()) {
        qCritical() << "Failed to start transaction for shipment:" << db.lastError().text();
        return false;
    }

    QByteArray shipment_quantity_enc = encryptData(QByteArray::number(shipment_quantity));
    QByteArray shipment_date_enc = encryptData(shipment_date.toString("yyyy-MM-dd").toUtf8());
    QByteArray recipient_enc = encryptData(recipient.toUtf8());

    QSqlQuery shipment_query(db);
    shipment_query.prepare("INSERT INTO shipment (shipment_date, quantity, id_product, recipient_name) "
                           "VALUES (?, ?, ?, ?)");

    shipment_query.addBindValue(shipment_date_enc);
    shipment_query.addBindValue(shipment_quantity_enc);
    shipment_query.addBindValue(product_id);
    shipment_query.addBindValue(recipient_enc);

    if (!shipment_query.exec()) {
        qCritical() << "Error INSERT into shipment (ID:" << product_id << "):" << shipment_query.lastError().text();
        db.rollback();
        return false;
    }

    QSqlQuery select_query(db);
    select_query.prepare("SELECT quantity FROM products WHERE id = ?");
    select_query.addBindValue(product_id);

    if (!select_query.exec() || !select_query.next()) {
        qCritical() << "Error SELECT quantity for update (ID:" << product_id << "):" << select_query.lastError().text();
        db.rollback();
        return false;
    }

    QByteArray current_quantity_enc = select_query.value(0).toByteArray();
    QByteArray current_quantity_bytes = decryptData(current_quantity_enc);

    bool ok;
    int current_quantity = current_quantity_bytes.toInt(&ok);
    if (!ok) {
        qCritical() << "Error decoding current quantity for product ID:" << product_id;
        db.rollback();
        return false;
    }

    int new_quantity = current_quantity - shipment_quantity;

    if (new_quantity < 0) {
        qWarning() << "Shipment failed: Not enough stock for product ID:" << product_id;
        db.rollback();
        return false;
    }

    QByteArray new_quantity_enc = encryptData(QByteArray::number(new_quantity));

    QSqlQuery update_query(db);
    update_query.prepare("UPDATE products SET quantity = ? WHERE id = ?");
    update_query.addBindValue(new_quantity_enc);
    update_query.addBindValue(product_id);

    if (!update_query.exec()) {
        qCritical() << "Error UPDATE products quantity (ID:" << product_id << "):" << update_query.lastError().text();
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        qCritical() << "Failed to commit transaction for shipment:" << db.lastError().text();
        return false;
    }

    qDebug() << "Shipment added. Product ID" << product_id << " quantity changed:" << current_quantity << "->" << new_quantity;
    return true;
}

bool ProductManager::addSupply(
    qint64 product_id,
    int supplies_quantity,
    const QDate& supplies_date,
    const QString& supplier)
{
    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return false;

    if (!db.transaction()) {
        qCritical() << "Failed to start transaction for supply:" << db.lastError().text();
        return false;
    }

    QByteArray supplies_quantity_enc = encryptData(QByteArray::number(supplies_quantity));
    QByteArray supplies_date_enc = encryptData(supplies_date.toString("yyyy-MM-dd").toUtf8());
    QByteArray supplier_enc = encryptData(supplier.toUtf8());

    QSqlQuery supplies_query(db);
    supplies_query.prepare("INSERT INTO supplies (supplies_date, quantity, id_product, supplier_name) "
                           "VALUES (?, ?, ?, ?)");

    supplies_query.addBindValue(supplies_date_enc);
    supplies_query.addBindValue(supplies_quantity_enc);
    supplies_query.addBindValue(product_id);
    supplies_query.addBindValue(supplier_enc);

    if (!supplies_query.exec()) {
        qCritical() << "Error INSERT into supplies (ID:" << product_id << "):" << supplies_query.lastError().text();
        db.rollback();
        return false;
    }

    QSqlQuery select_query(db);
    select_query.prepare("SELECT quantity FROM products WHERE id = ?");
    select_query.addBindValue(product_id);

    if (!select_query.exec() || !select_query.next()) {
        qCritical() << "Error SELECT quantity for update (ID:" << product_id << "):" << select_query.lastError().text();
        db.rollback();
        return false;
    }

    QByteArray current_quantity_enc = select_query.value(0).toByteArray();
    QByteArray current_quantity_bytes = decryptData(current_quantity_enc);

    bool ok;
    int current_quantity = current_quantity_bytes.toInt(&ok);
    if (!ok) {
        qCritical() << "Error decoding current quantity for product ID:" << product_id;
        db.rollback();
        return false;
    }

    int new_quantity = current_quantity + supplies_quantity;

    QByteArray new_quantity_enc = encryptData(QByteArray::number(new_quantity));

    QSqlQuery update_query(db);
    update_query.prepare("UPDATE products SET quantity = ? WHERE id = ?");
    update_query.addBindValue(new_quantity_enc);
    update_query.addBindValue(product_id);

    if (!update_query.exec()) {
        qCritical() << "Error UPDATE products quantity (ID:" << product_id << "):" << update_query.lastError().text();
        db.rollback();
        return false;
    }

    if (!db.commit()) {
        qCritical() << "Failed to commit transaction for supply:" << db.lastError().text();
        return false;
    }

    qDebug() << "Supply added. Product ID" << product_id << " quantity changed:" << current_quantity << "->" << new_quantity;
    return true;
}

QStringList ProductManager::findProduct(const QString& searchTerm)
{
    QStringList resultList;

    QSqlDatabase db = openConnection();
    if (!db.isOpen()) return resultList;

    QSqlQuery query(db);

    bool isId = false;
    qint64 id = searchTerm.toLongLong(&isId);

    qDebug() << "Поиск: ID=" << id << ", isId=" << isId;

    const QString selectQueryBase =
        "SELECT p.id, p.product_name, p.quantity AS current_quantity, p.price, p.about, "
        "s.shipment_date, s.quantity AS shipment_quantity, s.recipient_name, "
        "su.supplies_date, su.quantity AS supplies_quantity, su.supplier_name "
        "FROM products p "
        "LEFT JOIN shipment s ON p.id = s.id_product "
        "LEFT JOIN supplies su ON p.id = su.id_product ";

    QByteArray searchTerm_enc = encryptData(searchTerm.toUtf8());
    bool searchExecuted = false;

    if (isId && id > 0) {
        query.prepare(selectQueryBase + " WHERE p.id = ?");
        query.addBindValue(id);

        if (query.exec()) {
            if (query.size() > 0) {
                searchExecuted = true;
            }
        } else {
            qCritical() << "Error SELECT by ID:" << query.lastError().text();
            return resultList;
        }
    }

    if (!searchExecuted) {
        query.prepare(selectQueryBase + " WHERE p.product_name = ?");
        query.addBindValue(searchTerm_enc);

        if (query.exec()) {
            if (query.size() > 0) {
                searchExecuted = true;
            }
        } else {
            qCritical() << "Error SELECT by Encrypted Name:" << query.lastError().text();
            return resultList;
        }
    }

    if (!searchExecuted) {
        query.prepare(selectQueryBase + " WHERE su.supplier_name = ?");
        query.addBindValue(searchTerm_enc);

        if (!query.exec()) {
            qCritical() << "Error SELECT by Encrypted Supplier Name:" << query.lastError().text();
            return resultList;
        }

        searchExecuted = true;
    }

    QSqlQuery finalQuery(db);
    bool finalExecuted = false;

    if (isId && id > 0) {
        finalQuery.prepare(selectQueryBase + " WHERE p.id = ?");
        finalQuery.addBindValue(id);
        if (finalQuery.exec() && finalQuery.size() > 0) {
            finalExecuted = true;
        }
    }

    if (!finalExecuted) {
        finalQuery.prepare(selectQueryBase + " WHERE p.product_name = ?");
        finalQuery.addBindValue(searchTerm_enc);
        if (finalQuery.exec() && finalQuery.size() > 0) {
            finalExecuted = true;
        }
    }

    if (!finalExecuted) {
        finalQuery.prepare(selectQueryBase + " WHERE su.supplier_name = ?");
        finalQuery.addBindValue(searchTerm_enc);
        if (finalQuery.exec()) {

            finalExecuted = true;
        } else {
            qCritical() << "Error SELECT by Encrypted Supplier Name:" << finalQuery.lastError().text();
            return resultList;
        }
    }

    QSqlQuery* processQuery = &finalQuery;

    if (!finalExecuted) {
        qDebug() << "Поиск '" << searchTerm << "' завершен. Не найдено совпадений.";
        return resultList;
    }

    while (processQuery->next()) {
        QString row;
        row += QString("ID: %1 | ").arg(processQuery->value(0).toLongLong());

        for (int i = 1; i < 11; ++i) {
            QVariant value = processQuery->value(i);
            QString decryptedString;

            if (value.isNull() || value.toString().isEmpty()) {
                decryptedString = "N/A";
            } else {
                QByteArray encryptedData = value.toByteArray();
                QByteArray decryptedBytes = decryptData(encryptedData);
                decryptedString = QString::fromUtf8(decryptedBytes);
            }
            row += QString("%1 | ").arg(decryptedString);
        }
        resultList.append(row.trimmed());
    }

    qDebug() << "Поиск '" << searchTerm << "' завершен. Найдено" << resultList.size() << "записей.";
    return resultList;
}

QStringList ProductManager::getProductDataById(qint64 id)
{
    QStringList result;

    QSqlDatabase db = openConnection();
    if (!db.isOpen()) {
        qCritical() << "DB not open in getProductDataById";
        return result;
    }

    QSqlQuery query(db);

    // 1. Основные данные товара
    query.prepare("SELECT id, product_name, quantity, price, about FROM products WHERE id = ?");
    query.addBindValue(id);

    QString article, name, quantity, price, about;
    if (!query.exec() || !query.next()) {
        qWarning() << "Product not found:" << id;
        return result;
    }

    article  = QString::number(query.value(0).toLongLong());
    name     = decryptString(query.value(1));
    quantity = query.value(2).toString();
    price    = query.value(3).toString();
    about    = decryptString(query.value(4));

    // 2. Собираем поставки
    QString suppliesForGraph;   // yyyy-MM-dd:qty|
    QString suppliesForTable;   // yyyy-MM-dd|qty|Поставщик#

    query.prepare("SELECT supplies_date, quantity, supplier_name FROM supplies WHERE id_product = ? ORDER BY supplies_date DESC");
    query.addBindValue(id);
    if (query.exec()) {
        while (query.next()) {
            QDate date = query.value(0).toDate();
            double qty = query.value(1).toDouble();
            QString supplier = decryptString(query.value(2));

            if (date.isValid() && qty > 0) {
                suppliesForGraph += QString("%1:%2|").arg(date.toString("yyyy-MM-dd")).arg(qty, 0, 'f', 0);
                suppliesForTable += QString("%1|%2|%3#")
                                        .arg(date.toString("dd.MM.yyyy"))
                                        .arg(qty, 0, 'f', 0)
                                        .arg(supplier.isEmpty() ? "—" : supplier);
            }
        }
    }

    // 3. Собираем отгрузки
    QString shipmentsForGraph;
    QString shipmentsForTable;

    query.prepare("SELECT shipment_date, quantity, recipient_name FROM shipment WHERE id_product = ? ORDER BY shipment_date DESC");
    query.addBindValue(id);
    if (query.exec()) {
        while (query.next()) {
            QDate date = query.value(0).toDate();
            double qty = query.value(1).toDouble();
            QString recipient = decryptString(query.value(2));

            if (date.isValid() && qty > 0) {
                shipmentsForGraph += QString("%1:%2|").arg(date.toString("yyyy-MM-dd")).arg(qty, 0, 'f', 0);
                shipmentsForTable += QString("%1|%2|%3#")
                                         .arg(date.toString("dd.MM.yyyy"))
                                         .arg(qty, 0, 'f', 0)
                                         .arg(recipient.isEmpty() ? "—" : recipient);
            }
        }
    }

    // suppliesForGraph   = "2025-10-15:50|2025-11-01:30|";
    // suppliesForTable   = "15.10.2025|50|ООО Рога и Копыта#01.11.2025|30|Поставщик №2#";
    // shipmentsForGraph  = "2025-11-20:20|2025-12-05:15|";
    // shipmentsForTable  = "20.11.2025|20|ИП Иванов#05.12.2025|15|Магазин Детский мир#";

    // 4. Заполняем результат (ровно 9 элементов!)
    result << article           // 0
           << name              // 1
           << about             // 2
           << price             // 3
           << quantity          // 4
           << suppliesForGraph  // 5 — для графика
           << suppliesForTable  // 6 — для таблицы
           << shipmentsForGraph // 7
           << shipmentsForTable; // 8

    qDebug() << result;


              qDebug() << "getProductDataById(" << id << ") returned" << result.size() << "items";
    qDebug() << "Supplies graph:" << suppliesForGraph;
    qDebug() << "Shipments graph:" << shipmentsForGraph;

    return result;
}

QString ProductManager::decryptString(const QVariant &v)
{
    if (v.isNull() || v.toString().isEmpty()) return "N/A";
    QByteArray data = v.toByteArray();
    QByteArray decrypted = decryptData(data);  // твоя функция
    return QString::fromUtf8(decrypted);
}
