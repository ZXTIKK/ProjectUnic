#include "outputexcel.h"
#include <QFile>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include <QByteArray>
#include <QtDebug>


outputexcel::outputexcel() {}

bool outputexcel::exportAllToCsv(const QString &filePath, const QString &fileName)
{
    QString fullPath = QString("%1/%2.csv").arg(filePath, fileName);

    QSqlQuery query("SELECT Id, product_name, about, price, quantity FROM products");

    if (!query.exec()) {
        qWarning() << "outputexcel Error: DB query failed for exportAllToCsv:" << query.lastError().text();
        return false;
    }

    return createCsvDocument(query, fullPath);
}


bool outputexcel::exportToCsvById(int id, const QString &filePath, const QString &fileName)
{
    QString fullPath = QString("%1/%2.csv").arg(filePath, fileName);

    QSqlQuery query;
    query.prepare("SELECT Id, product_name, about, price, quantity FROM products WHERE Id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        qWarning() << "outputexcel Error: DB query failed for exportToCsvById:" << query.lastError().text();
        return false;
    }

    if (query.size() == 0) {
        qWarning() << "outputexcel Warning: Product with ID =" << id << " not found for export.";
        return false;
    }

    return createCsvDocument(query, fullPath);
}


bool outputexcel::createCsvDocument(QSqlQuery &query, const QString &fullPath)
{
    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "outputexcel Error: Failed to open file for writing:" << fullPath;
        return false;
    }

    QByteArray fileData;
    fileData.append('\xEF');
    fileData.append('\xBB');
    fileData.append('\xBF');

    QString separator = ";";

    QStringList headers = {"Id", "product_name", "about", "price", "quantity"};

    fileData.append(headers.join(separator).toUtf8());
    fileData.append('\n');

    query.first();
    query.previous();

    int numRows = 0;
    while (query.next()) {
        QStringList rowData;
        int numCols = 5;

        for (int col = 0; col < numCols; ++col) {
            QString value = query.value(col).toString();
            if (col >= 1 && col <= 4) {
                QByteArray decodedBytes = QByteArray::fromBase64(value.toUtf8());
                value = QString::fromUtf8(decodedBytes);
                value.replace(separator, " ").trimmed();
            }

            rowData << "\"" + value + "\"";
        }

        fileData.append(rowData.join(separator).toUtf8());
        fileData.append('\n');
        numRows++;
    }

    if (file.write(fileData) == -1) {
        qWarning() << "outputexcel Error: Failed to write data block to file.";
        file.close();
        return false;
    }

    file.close();

    if (numRows == 0) {
        qWarning() << "outputexcel Warning: Export finished, but no data was written.";
        return false;
    }

    qInfo() << "outputexcel Success: Data successfully exported to" << fullPath;
    return true;
}
