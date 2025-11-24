// mainwindow.h
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void updateLineEditStyle(QLineEdit*,bool);

private slots:
    void onLoginButtonClicked();

private:
    QLineEdit* lineEditLogin;
    QLineEdit* lineEditPassword;
    QPushButton* btnAuth;
    QLabel* lableInfo;
};

#endif // MAINWINDOW_H
