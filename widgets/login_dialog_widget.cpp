#include "login_dialog_widget.h"

LoginDialogWidget::LoginDialogWidget(QObject *parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(20, 20, 20, 20);
  layout->setSpacing(16);

  QLabel *titleLabel = new QLabel("Train Simulation Login", this);
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setStyleSheet("QLabel{" + TextStyle::BodyBigBold() +
                            "colors: " + Colors::StandardBlack.name() +
                            ";"
                            "}");
  layout->addWidget(titleLabel);

  m_usernameInput = new InputWidget(this, InputType("field", "Username"));
  m_passwordInput = new InputWidget(this, InputType("field", "Password"));
  m_passwordInput->setEchoMode(QLineEdit::Password);
  m_usernameInput->setPlaceholderText("Enter username...");
  m_passwordInput->setPlaceholderText("Enter password...");
  m_usernameInput->setSize(300, 32);
  m_passwordInput->setSize(300, 32);

  layout->addWidget(m_usernameInput);
  layout->addWidget(m_passwordInput);

  m_loginButton = new ButtonAction(this, "Login", "no");
  m_loginButton->setEnabled(true);
  m_loginButton->setSize(300, 44);
  layout->addWidget(m_loginButton);
  connect(m_loginButton, &QPushButton::clicked, this,
          &LoginDialogWidget::onLoginButtonClicked);
  setFixedSize(350, 250);
  setWindowTitle("Login");
  setWindowFlags(Qt::Dialog | Qt::WindowTitleHint | Qt::CustomizeWindowHint |
                 Qt::WindowCloseButtonHint);
}

void LoginDialogWidget::onLoginButtonClicked() {
  QString username = m_usernameInput->getTextValue();
  QString password = m_passwordInput->getTextValue();
  emit loginAttempt(username, password);
}