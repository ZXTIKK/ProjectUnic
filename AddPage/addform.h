#ifndef ADDFORM_H
#define ADDFORM_H

#include <QWidget>

namespace Ui {
class AddForm;
}

class AddForm : public QWidget
{
    Q_OBJECT

public:
    explicit AddForm(QWidget *parent = nullptr);
    ~AddForm();

private:
    Ui::AddForm *ui;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void Cancel();

private slots:
    void on_pushButton_Cancel_clicked();
    void on_pushButton_Add_clicked();
};

#endif // ADDFORM_H
