#ifndef OUTPUTWORD_H
#define OUTPUTWORD_H
#include <QTextDocument>
#include <QTextTable>
#include <QTextCursor>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QFileDialog>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QSqlQuery>

class OutputWord
{
public:
    OutputWord();
    bool exportById(int id, const QString &filePath, const QString &fileName);
    bool exportAll(const QString &filePath, const QString &fileName);

private:
    bool createDocument(QSqlQuery &query, const QString &fullPath);
};

#endif // OUTPUTWORD_H
