#ifndef EDITFORM_H
#define EDITFORM_H

#include <QWidget>

namespace Ui {
class EditForm;
}

class EditForm : public QWidget
{
    Q_OBJECT

public:
    explicit EditForm(QWidget *parent = nullptr);
    ~EditForm();

private:
    Ui::EditForm *ui;
    static qint64 id;

private slots:
    void updateData(qint64 id);
    void on_pushButton_back_clicked();
    void on_pushButton_conf_click();

signals:
    void Cancel();
};

#endif // EDITFORM_H
