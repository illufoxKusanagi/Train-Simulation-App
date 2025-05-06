#include "login_dialog_widget.h"

LoginDialogWidget::LoginDialogWidget(QObject *parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(20, 20, 20, 20);
  layout->setSpacing(15);

  QLabel *titleLabel = new QLabel("Train Simulation Login", this);
  titleLabel->setAlignment(Qt::AlignCenter);
  QFont titleFont = titleLabel->font();
  titleFont.setPointSize(14);
  titleFont.setBold(true);
  titleLabel->setFont(titleFont);
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
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  m_loginButton = new ButtonAction(this, "Login", "no");
  m_loginButton->setEnabled(true);
  m_cancelButton = new ButtonAction(this, "Cancel", "no");
  m_cancelButton->setEnabled(true);

  buttonLayout->addWidget(m_loginButton);
  buttonLayout->addWidget(m_cancelButton);
  layout->addLayout(buttonLayout);

  connect(m_loginButton, &QPushButton::clicked, this,
          &LoginDialogWidget::onLoginButtonClicked);
  connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
  setFixedSize(350, 250);
  setWindowTitle("Login");
}

void LoginDialogWidget::onLoginButtonClicked() {
  QString username = m_usernameInput->getTextValue();
  QString password = m_passwordInput->getTextValue();
  emit loginAttempt(username, password);
}