#ifndef BASICFORM_H
#define BASICFORM_H

#include <QWidget>

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

private slots:
    void on_pushButton_add_clicked();
    void updateDate();

signals:
    void PageAdd();
};

#endif // BASICFORM_H
