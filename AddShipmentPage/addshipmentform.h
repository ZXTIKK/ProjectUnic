#ifndef ADDSHIPMENTFORM_H
#define ADDSHIPMENTFORM_H

#include <QWidget>

namespace Ui {
class AddShipmentForm;
}

class AddShipmentForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddShipmentForm(QWidget *parent = nullptr);
    ~AddShipmentForm();

private:
    Ui::AddShipmentForm *ui;

public slots:
    void getData();

private slots:
    void on_pushButton_back_clicked();
    void on_pushButton_conf_clicked();

signals:
    void cancelAddShipment();
};

#endif // ADDSHIPMENTFORM_H
