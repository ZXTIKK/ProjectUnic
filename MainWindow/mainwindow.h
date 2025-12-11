// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QStackedWidget>

#include "../LoginPage/loginform.h"
#include "../BasicPage/basicform.h"
#include "../AddPage/addform.h"
#include "../EditPage/editform.h"
#include "../Settings/settingsform.h"
#include "../AddSupplyPage/addsupplyform.h"
#include "../AddShipmentPage/addshipmentform.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    static LoginForm* getLoginForm();
    static BasicForm* getBasicForm();
    static AddForm* getAddForm();
    static EditForm* getEditForm();
    static SettingsForm* getSettingsForm();
    static AddShipmentForm* getAddShipmentForm();

private slots:
    void switchToBasicForm();
    void switchToAddForm();
    void cancelFromAdd();
    void switchToEditForm(qint64 id);
    void cancelFromEdit();
    void switchToSettingsPage();
    void switchToLoginPage();
    void switchToAddSupplyForm();
    void switchToAddShipmentForm();

private:
    QStackedWidget *stackedWidget;
    int basicIndex;
    int loginIndex;
    int addIndex;
    int editIndex;
    int settingsIndex;
    int addSupplyIndex;
    int addShipmentIndex;
    static LoginForm *loginForm;
    static BasicForm *basicForm;
    static AddForm *addForm;
    static EditForm *editForm;
    static SettingsForm *settingsForm;
    static AddSupplyForm *addSupplyForm;
    static AddShipmentForm *addShipmentForm;
};

#endif // MAINWINDOW_H
