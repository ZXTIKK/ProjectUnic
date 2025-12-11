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

LoginForm* MainWindow::loginForm = nullptr;
BasicForm* MainWindow::basicForm = nullptr;
AddForm* MainWindow::addForm = nullptr;
EditForm* MainWindow::editForm = nullptr;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    loginForm = new LoginForm();
    basicForm = new BasicForm();
    addForm = new AddForm();
    editForm = new EditForm();

    loginIndex = stackedWidget->addWidget(loginForm);
    basicIndex = stackedWidget->addWidget(basicForm);
    addIndex = stackedWidget->addWidget(addForm);
    editIndex = stackedWidget->addWidget(editForm);

    stackedWidget->setCurrentIndex(loginIndex);
    setWindowTitle("Авторизация");

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
