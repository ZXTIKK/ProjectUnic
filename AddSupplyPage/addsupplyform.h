#ifndef ADDSUPPLYFORM_H
#define ADDSUPPLYFORM_H

#include <QWidget>

namespace Ui {
class AddSupplyForm;
}

class AddSupplyForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddSupplyForm(QWidget *parent = nullptr);
    ~AddSupplyForm();

private:
    Ui::AddSupplyForm *ui;

public slots:
    void getData();

private slots:
    void on_pushButton_back_clicked();
    void on_pushButton_conf_clicked();


signals:
    void cancelAddSupply();
};

#endif // ADDSUPPLYFORM_H
