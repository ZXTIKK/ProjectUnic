#include "basicform.h"
#include "ui_basicform.h"

BasicForm::BasicForm(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BasicForm)
{
    ui->setupUi(this);
}

BasicForm::~BasicForm()
{
    delete ui;
}
