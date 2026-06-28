#include "outputexcel.h"
#include "../Authentication/authentication.h"

// ← ЭТИ ЗАГОЛОВКИ ОБЯЗАТЕЛЬНЫ!
#include <QFile>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>
#include <QByteArray>
#include <QDebug>

OutputExcel::OutputExcel() = default;

bool OutputExcel::exportAllToCsv(const QString &filePath, const QString &fileName)
{
    QString fullPath = QString("%1/%2.csv").arg(filePath, fileName);

    QSqlDatabase db = Authentication::getConnect();
    if (!db.isValid() || !db.isOpen()) {
        qWarning() << "OutputExcel: Нет подключения к БД!";
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT Id, product_name, about, price, quantity FROM products ORDER BY Id")) {
        qWarning() << "OutputExcel:" << query.lastError().text();
        return false;
    }

    return createCsvDocument(query, fullPath);
}

// exportToCsvById оставляем как есть или удаляем, если не нужен

bool OutputExcel::createCsvDocument(QSqlQuery &query, const QString &fullPath)
{
    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qWarning() << "Не могу открыть файл:" << fullPath;
        return false;
    }

    QByteArray data;
    data.append("\xEF\xBB\xBF"); // BOM

    const QString sep = ";";
    const QStringList headers = {"Артикул", "Наименование", "Описание", "Цена", "Количество"};
    data.append(headers.join(sep).toUtf8() + "\n");

    while (query.next()) {
        QStringList row;
        for (int i = 0; i < 5; ++i) {
            QString val = query.value(i).toString();
            // Столбцы 1..4 (name, about, price, quantity) хранятся в Base64.
            if (i >= 1 && i <= 4) {
                val = QString::fromUtf8(QByteArray::fromBase64(val.toUtf8()));
            }
            val.replace('"', "\"\"");
            val.replace(sep, " ");
            row << "\"" + val.trimmed() + "\"";
        }
        data.append(row.join(sep).toUtf8() + "\n");
    }

    file.write(data);
    file.close();
    qInfo() << "CSV сохранён:" << fullPath;
    return true;
}
