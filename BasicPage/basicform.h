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
};

#endif // BASICFORM_H
