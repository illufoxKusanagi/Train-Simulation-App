#ifndef LOGIN_DIALOG_WIDGET_H
#define LOGIN_DIALOG_WIDGET_H

#include "input_widget.h"
#include "message_box_widget.h"
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

class LoginDialogWidget : QWidget {
  Q_OBJECT

private slots:
  void onUsernameChanged(const QString &username);
  void onPasswordChanged(const QString &password);

public:
  explicit LoginDialogWidget(QObject *parent = nullptr);

private:
  InputWidget *m_usernameInput;
  InputWidget *m_passwordInput;
};

#endif // LOGIN_DIALOG_WIDGET_H