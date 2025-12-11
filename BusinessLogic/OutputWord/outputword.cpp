#include "outputword.h"
#include "../Authentication/authentication.h"

// ← ВСЕ НУЖНЫЕ ЗАГОЛОВКИ ЗДЕСЬ!
#include <QTextDocument>
#include <QTextCursor>
#include <QTextTable>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDateTime>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QSqlError>

OutputWord::OutputWord() = default;

bool OutputWord::exportAll(const QString &filePath, const QString &fileName)
{
    QString fullPath = QString("%1/%2.html").arg(filePath, fileName);

    QSqlDatabase db = Authentication::getConnect();
    if (!db.isOpen()) {
        QMessageBox::critical(nullptr, "Ошибка", "Нет подключения к БД");
        return false;
    }

    QSqlQuery query(db);
    if (!query.exec("SELECT Id, product_name, about, price, quantity FROM products ORDER BY Id")) {
        QMessageBox::critical(nullptr, "Ошибка БД", query.lastError().text());
        return false;
    }

    return createDocument(query, fullPath);
}

bool OutputWord::createDocument(QSqlQuery &query, const QString &fullPath)
{
    QTextDocument doc;
    QTextCursor cursor(&doc);

    // Заголовок
    QTextCharFormat titleFmt;
    titleFmt.setFontPointSize(20);
    titleFmt.setFontWeight(QFont::Bold);
    titleFmt.setForeground(QColor("#2c3e50"));
    cursor.insertText("Полный отчёт по складу", titleFmt);
    cursor.insertBlock();
    cursor.insertText("Дата: " + QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm"));
    cursor.insertBlock();
    cursor.insertBlock();

    int rows = 0;
    while (query.next()) rows++;
    query.seek(-1); // на начало

    if (rows == 0) {
        cursor.insertText("Нет данных");
    } else {
        QTextTableFormat tableFmt;
        tableFmt.setBorder(1);
        tableFmt.setCellPadding(8);
        tableFmt.setWidth(QTextLength(QTextLength::PercentageLength, 100));

        QTextTable *table = cursor.insertTable(rows + 1, 5, tableFmt);

        QTextCharFormat headFmt;
        headFmt.setBackground(QColor("#3498db"));
        headFmt.setForeground(Qt::white);
        headFmt.setFontWeight(QFont::Bold);

        QStringList heads = {"Артикул", "Наименование", "Описание", "Цена", "Количество"};
        for (int c = 0; c < 5; ++c) {
            table->cellAt(0, c).firstCursorPosition().insertText(heads[c], headFmt);
        }

        int r = 1;
        while (query.next()) {
            for (int c = 0; c < 5; ++c) {
                QString val = query.value(c).toString();
                if (c >= 1 && c <= 2) {
                    val = QString::fromUtf8(QByteArray::fromBase64(val.toUtf8()));
                }
                table->cellAt(r, c).firstCursorPosition().insertText(val);
            }
            r++;
        }
    }

    QFile file(fullPath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "Ошибка", "Не удалось сохранить файл");
        return false;
    }

    QTextStream out(&file);
    out.setEncoding(QStringConverter::Utf8);
    out << doc.toHtml(); // ← теперь работает!
    file.close();

    QMessageBox::information(nullptr, "Готово", "Отчёт сохранён:\n" + fullPath);
    return true;
}
