#ifndef OUTPUTEXCEL_H
#define OUTPUTEXCEL_H

#include <QString>

class QSqlQuery;

class OutputExcel
{
public:
    OutputExcel();
    ~OutputExcel() = default;

    // Экспорт всех товаров
    bool exportAllToCsv(const QString &filePath, const QString &fileName);

    // Экспорт одного товара по Id (можно не использовать, но оставим)
    bool exportToCsvById(int id, const QString &filePath, const QString &fileName);

private:
    bool createCsvDocument(QSqlQuery &query, const QString &fullPath);
};

#endif // OUTPUTEXCEL_H
