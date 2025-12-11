#ifndef ABOUTPRODUCTFORM_H
#define ABOUTPRODUCTFORM_H

#include <QWidget>

namespace Ui {
class AboutProductForm;
}

class AboutProductForm : public QWidget
{
    Q_OBJECT

public:
    explicit AboutProductForm(QWidget *parent = nullptr);
    ~AboutProductForm();

private:
    Ui::AboutProductForm *ui;
};

#endif // ABOUTPRODUCTFORM_H
