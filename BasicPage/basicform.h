#ifndef BASICFORM_H
#define BASICFORM_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include <QPointer>

namespace Ui {
class BasicForm;
}

class BasicForm : public QWidget
{
    Q_OBJECT

public:
    explicit BasicForm(QWidget *parent = nullptr);
    ~BasicForm();

private:
    Ui::BasicForm *ui;
    QPointer<QTableWidget> m_tableProducts;
    QPointer<QLabel> m_labelNotFound;
    void clearScrollArea();
    void addDataInTable(QStringList products);

private slots:
    void on_pushButton_add_clicked();
    void updateDate();
    void on_detailsButton_clicked(qint64 id);
    void on_deleteButton_clicked(qint64 id);
    void on_editButton_clicked(qint64 id);
    void on_pushButton_find_clicked();
    void on_pushButton_deleteFind_clicked();

signals:
    void PageAdd();
};

#endif // BASICFORM_H
