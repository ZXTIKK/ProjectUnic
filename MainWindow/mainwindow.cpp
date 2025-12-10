#include "mainwindow.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QDebug>
#include <QStackedWidget>

#include "../LoginPage/loginform.h"
#include "../BasicPage/basicform.h"
#include "../AddPage/addform.h"

LoginForm* MainWindow::loginForm = nullptr;
BasicForm* MainWindow::basicForm = nullptr;
AddForm* MainWindow::addForm = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    loginForm = new LoginForm();
    basicForm = new BasicForm();
    addForm = new AddForm();

    loginIndex = stackedWidget->addWidget(loginForm);
    basicIndex = stackedWidget->addWidget(basicForm);
    addIndex = stackedWidget->addWidget(addForm);

    stackedWidget->setCurrentIndex(loginIndex);
    setWindowTitle("Авторизация");

    connect(loginForm, &LoginForm::loginSuccessful,
            this, &MainWindow::switchToBasicForm);
    connect(basicForm, &BasicForm::PageAdd,
            this, &MainWindow::switchToAddForm);
    connect(addForm, &AddForm::Cancel,
            this, &MainWindow::cancelFromAdd);

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

LoginForm* MainWindow::getLoginForm(){
    return MainWindow::loginForm;
}

BasicForm* MainWindow::getBasicForm(){
    return MainWindow::basicForm;
}

AddForm* MainWindow::getAddForm(){
    return MainWindow::addForm;
}
