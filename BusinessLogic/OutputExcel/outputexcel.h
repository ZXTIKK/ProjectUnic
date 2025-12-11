#ifndef outputexcel_H
#define outputexcel_H

#include <QString>
class outputexcel
{
public:
    outputexcel();

    /**
     * @brief
     * @param
     * @param
     * @param
     * @return
     */
    bool exportToCsvById(int id, const QString &filePath, const QString &fileName);

    /**
     * @brief
     * @param
     * @param
     * @return
     */
    bool exportAllToCsv(const QString &filePath, const QString &fileName);

private:
    bool createCsvDocument(class QSqlQuery &query, const QString &fullPath);
};

#endif // outputexcel_H
