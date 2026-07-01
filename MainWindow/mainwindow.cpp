#include "mainwindow.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
#include <QStackedWidget>
#include <QFileDialog>

#include "../LoginPage/loginform.h"
#include "../BasicPage/basicform.h"
#include "../AddPage/addform.h"
#include "../EditPage/editform.h"
#include "../Settings/settingsform.h"
#include "../AddSupplyPage/addsupplyform.h"
#include "../AddShipmentPage/addshipmentform.h"

LoginForm* MainWindow::loginForm = nullptr;
BasicForm* MainWindow::basicForm = nullptr;
AddForm* MainWindow::addForm = nullptr;
EditForm* MainWindow::editForm = nullptr;
SettingsForm* MainWindow::settingsForm = nullptr;
AddSupplyForm* MainWindow::addSupplyForm = nullptr;
AddShipmentForm* MainWindow::addShipmentForm = nullptr;
AboutForm* MainWindow::aboutForm = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    loginForm = new LoginForm();
    basicForm = new BasicForm();
    addForm = new AddForm();
    editForm = new EditForm();
    settingsForm = new SettingsForm();
    addSupplyForm = new AddSupplyForm();
    addShipmentForm = new AddShipmentForm();
    aboutForm = new AboutForm();

    loginIndex = stackedWidget->addWidget(loginForm);
    basicIndex = stackedWidget->addWidget(basicForm);
    addIndex = stackedWidget->addWidget(addForm);
    editIndex = stackedWidget->addWidget(editForm);
    settingsIndex = stackedWidget->addWidget(settingsForm);
    addSupplyIndex = stackedWidget->addWidget(addSupplyForm);
    addShipmentIndex = stackedWidget->addWidget(addShipmentForm);
    aboutIndex = stackedWidget->addWidget(aboutForm);

    if(SettingsForm::loadSettingsFromRegistry())
    {
        stackedWidget->setCurrentIndex(loginIndex);
        setWindowTitle("Авторизация");
    }else{
        stackedWidget->setCurrentIndex(settingsIndex);
        setWindowTitle("Настройки");
    }

    connect(loginForm, &LoginForm::loginSuccessful,
            this, &MainWindow::switchToBasicForm);
    connect(basicForm, &BasicForm::PageAdd,
            this, &MainWindow::switchToAddForm);
    connect(addForm, &AddForm::Cancel,
            this, &MainWindow::cancelFromAdd);
    connect(basicForm, &BasicForm::switchToEditForm,
            this, &MainWindow::switchToEditForm);
    connect(editForm, &EditForm::Cancel,
            this, &MainWindow::cancelFromEdit);
    connect(addForm, &AddForm::Cancel,
            basicForm, &BasicForm::updateDate);
    connect(editForm, &EditForm::Cancel,
            basicForm, &BasicForm::updateDate);
    connect(loginForm, &LoginForm::settingsPage,
            this, &MainWindow::switchToSettingsPage);
    connect(settingsForm, &SettingsForm::Cancel_Settings,
            this, &MainWindow::switchToLoginPage);
    connect(basicForm, &BasicForm::switchToAddSupplyForm,
            this,&MainWindow::switchToAddSupplyForm);
    connect(addSupplyForm, &AddSupplyForm::cancelAddSupply,
            this, &MainWindow::cancelFromEdit);
    connect(addSupplyForm, &AddSupplyForm::cancelAddSupply,
            basicForm, &BasicForm::updateDate);
    connect(basicForm, &BasicForm::switchToAddShipmentForm,
            this,&MainWindow::switchToAddShipmentForm);
    connect(addShipmentForm, &AddShipmentForm::cancelAddShipment,
            this, &MainWindow::switchToBasicForm);
    connect(basicForm, &BasicForm::switchToAbout,
            this, &MainWindow::switchToAboutForm);
    connect(aboutForm, &AboutForm::cancelAbout,
            this, &MainWindow::switchToBasicForm);
    connect(addShipmentForm, &AddShipmentForm::cancelAddShipment,
            basicForm, &BasicForm::updateDate);


}

MainWindow::~MainWindow() {}

void MainWindow::switchToBasicForm()
{
    stackedWidget->setCurrentIndex(basicIndex);
    setWindowTitle("Товары");
}

void MainWindow::switchToAddForm()
{
    stackedWidget->setCurrentIndex(addIndex);
    setWindowTitle("Добавить товар");
}

void MainWindow::cancelFromAdd()
{
    stackedWidget->setCurrentIndex(basicIndex);
    setWindowTitle("Товары");
}

void MainWindow::cancelFromEdit(){
    stackedWidget->setCurrentIndex(basicIndex);
    setWindowTitle("Товары");
}

void MainWindow::switchToEditForm(qint64 id)
{
    stackedWidget->setCurrentIndex(editIndex);
    setWindowTitle("Изменить товар");
    qDebug() << id;
}


LoginForm* MainWindow::getLoginForm(){
    return MainWindow::loginForm;
}

BasicForm* MainWindow::getBasicForm(){
    return MainWindow::basicForm;
}

AddForm* MainWindow::getAddForm(){
    return MainWindow::addForm;
}

EditForm* MainWindow::getEditForm(){
    return MainWindow::editForm;
}

void MainWindow::switchToSettingsPage()
{
    stackedWidget->setCurrentIndex(settingsIndex);
    setWindowTitle("Настройки");
}

void MainWindow::switchToLoginPage(){
    stackedWidget->setCurrentIndex(loginIndex);
    setWindowTitle("Авторизация");
}

void MainWindow::switchToAddSupplyForm()
{
    stackedWidget->setCurrentIndex(addSupplyIndex);
    setWindowTitle("Поставка");
}

void MainWindow::switchToAddShipmentForm()
{
    stackedWidget->setCurrentIndex(addShipmentIndex);
    setWindowTitle("Отгрузка");
}

void MainWindow::switchToAboutForm()
{
    stackedWidget->setCurrentIndex(aboutIndex);
    setWindowTitle("О товарах");
}
