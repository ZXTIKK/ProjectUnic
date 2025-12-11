#include "outputword.h"
#include <QTextDocument>
#include <QTextTable>
#include <QTextCursor>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QSqlError>

OutputWord::OutputWord() {}

bool OutputWord::exportAll(const QString &filePath, const QString &fileName)
{
    QString fullPath = QString("%1/%2.html").arg(filePath, fileName);

    QSqlQuery query("SELECT Id, product_name, about, price, quantity FROM products");

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "Ошибка БД",
                              "Не удалось выполнить запрос: " + query.lastError().text());
        return false;
    }

    return createDocument(query, fullPath);
}

bool OutputWord::exportById(int id, const QString &filePath, const QString &fileName)
{
    QString fullPath = QString("%1/%2.html").arg(filePath, fileName);

    QSqlQuery query;
    query.prepare("SELECT Id, product_name, about, price, quantity FROM products WHERE Id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        QMessageBox::critical(nullptr, "Ошибка БД",
                              "Не удалось выполнить запрос: " + query.lastError().text());
        return false;
    }

    if (query.size() == 0) {
        QMessageBox::information(nullptr, "Нет данных",
                                 "Продукт с ID = " + QString::number(id) + " не найден.");
        return false;
    }

    return createDocument(query, fullPath);
}

bool OutputWord::createDocument(QSqlQuery &query, const QString &fullPath)
{
    QTextDocument document;
    QTextCursor cursor(&document);

    QTextCharFormat formatTitle;
    formatTitle.setFontPointSize(16);
    formatTitle.setFontWeight(QFont::Bold);
    cursor.insertText("Отчет о продуктах", formatTitle);
    cursor.insertBlock();
    cursor.insertBlock();

    int numRows = 0;
    while (query.next()) {
        numRows++;
    }
    query.first();
    query.previous();

    if (numRows == 0) {
        cursor.insertText("Нет данных для экспорта.");
    }

    int numCols = 5;
    QTextTableFormat tableFormat;
    tableFormat.setBorder(1);
    tableFormat.setCellSpacing(0);
    tableFormat.setCellPadding(5);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));

    QTextTable *table = cursor.insertTable(numRows + 1, numCols, tableFormat);

    QTextCharFormat formatHeader;
    formatHeader.setBackground(QColor("#E0E0E0"));
    formatHeader.setFontWeight(QFont::Bold);

    QStringList headers = {"Id", "product_name", "about", "price", "quantity"};

    for (int i = 0; i < numCols; ++i) {
        QTextTableCell cell = table->cellAt(0, i);
        QTextCursor cellCursor = cell.firstCursorPosition();
        cellCursor.insertText(headers.at(i), formatHeader);
    }

    int row = 1;
    while (query.next()) {
        for (int col = 0; col < numCols; ++col) {
            QTextTableCell cell = table->cellAt(row, col);
            QTextCursor cellCursor = cell.firstCursorPosition();
            cellCursor.insertText(query.value(col).toString());
        }
        row++;
    }

    QFile file(fullPath);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << document.toHtml();
        file.close();

        QMessageBox::information(nullptr, "Экспорт успешен",
                                 "Данные успешно экспортированы в файл: " + fullPath);
        return true;
    } else {
        QMessageBox::critical(nullptr, "Ошибка файла",
                              "Не удалось открыть файл для записи: " + fullPath);
        return false;
    }
}
