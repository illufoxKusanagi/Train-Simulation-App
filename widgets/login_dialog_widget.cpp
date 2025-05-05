#include "login_dialog_widget.h"

LoginDialogWidget::LoginDialogWidget(QObject *parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);
  layout->setAlignment(Qt::AlignLeft);
  m_usernameInput = new InputWidget(this, InputType("field", "Username"));
  m_passwordInput = new InputWidget(this, InputType("field", "Password"));
  m_usernameInput->setPlaceholderText("Enter username...");
  m_passwordInput->setPlaceholderText("Enter password...");
  m_passwordInput->setEchoMode(QLineEdit::Password);
  layout->addWidget(m_usernameInput);
  layout->addWidget(m_passwordInput);
  //   connect(m_usernameInput, &InputWidget::valueChanged, this,
  //           &LoginDialogWidget::onUsernameChanged);
  //   connect(m_passwordInput, &InputWidget::valueChanged, this,
  //           &LoginDialogWidget::onPasswordChanged);
}

void LoginDialogWidget::onUsernameChanged(const QString &username) {
  // Handle username change
}
void LoginDialogWidget::onPasswordChanged(const QString &password) {
  // Handle password change
}