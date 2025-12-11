#ifndef OUTPUTWORD_H
#define OUTPUTWORD_H

#include <QString>

class QSqlQuery;

class OutputWord
{
public:
    OutputWord();
    ~OutputWord() = default;

    bool exportAll(const QString &filePath, const QString &fileName);
    bool exportById(int id, const QString &filePath, const QString &fileName);

private:
    bool createDocument(QSqlQuery &query, const QString &fullPath);
};

#endif // OUTPUTWORD_H
