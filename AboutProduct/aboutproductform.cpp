#include "aboutproductform.h"
#include "ui_aboutproductform.h"

AboutProductForm::AboutProductForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AboutProductForm)
{
    ui->setupUi(this);
}

AboutProductForm::~AboutProductForm()
{
    delete ui;
}
