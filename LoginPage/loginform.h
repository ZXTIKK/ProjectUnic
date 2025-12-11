#ifndef LOGINFORM_H
#define LOGINFORM_H

#include <QWidget>
#include <QKeyEvent>

namespace Ui { class LoginForm; }

class LoginForm : public QWidget
{
    Q_OBJECT

public:
    explicit LoginForm(QWidget *parent = nullptr);
    ~LoginForm();

private slots:
    void on_pushButton_auth_clicked();
    void on_pushButton_settings_clicked();

private:
    Ui::LoginForm *ui;

signals:
    void loginSuccessful();
    void settingsPage();

private:
    void keyPressEvent(QKeyEvent *event) override;
};

#endif // LOGINFORM_H
