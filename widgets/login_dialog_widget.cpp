#include "login_dialog_widget.h"

LoginDialogWidget::LoginDialogWidget(QObject *parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(20, 20, 20, 20);
  layout->setSpacing(15);

  // Title
  QLabel *titleLabel = new QLabel("Train Simulation Login", this);
  titleLabel->setAlignment(Qt::AlignCenter);
  QFont titleFont = titleLabel->font();
  titleFont.setPointSize(14);
  titleFont.setBold(true);
  titleLabel->setFont(titleFont);
  layout->addWidget(titleLabel);

  // Input fields
  m_usernameInput = new InputWidget(this, InputType("field", "Username"));
  m_passwordInput = new InputWidget(this, InputType("field", "Password"));
  m_usernameInput->setPlaceholderText("Enter username...");
  m_passwordInput->setPlaceholderText("Enter password...");
  m_passwordInput->setEchoMode(QLineEdit::Password);

  layout->addWidget(m_usernameInput);
  layout->addWidget(m_passwordInput);

  // Button container
  QHBoxLayout *buttonLayout = new QHBoxLayout();

  // Create buttons
  m_loginButton = new ButtonAction(this, "Login", "no");
  m_loginButton->setEnabled(true);
  m_cancelButton = new ButtonAction(this, "Cancel", "no");
  m_cancelButton->setEnabled(true);

  buttonLayout->addWidget(m_loginButton);
  buttonLayout->addWidget(m_cancelButton);
  layout->addLayout(buttonLayout);

  // Connect signals and slots
  connect(m_loginButton, &QPushButton::clicked, this,
          &LoginDialogWidget::onLoginButtonClicked);
  connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
  //   connect(m_usernameInput, &InputWidget::valueChanged, this,
  //           &LoginDialogWidget::onUsernameChanged);
  //   connect(m_passwordInput, &InputWidget::valueChanged, this,
  //           &LoginDialogWidget::onPasswordChanged);

  setFixedSize(350, 250);
  setWindowTitle("Login");
}

void LoginDialogWidget::onLoginButtonClicked() {
  QString username;
  QString password;
  username = "admin";
  password = "admin";
  // Emit signal with login credentials
  emit loginAttempt(username, password);
}

void LoginDialogWidget::onUsernameChanged(const QString &username) {
  // Enable login button only if both fields have content
  //   m_loginButton->setEnabled(!username.isEmpty() &&
  //                             !m_passwordInput->text().isEmpty());
}

void LoginDialogWidget::onPasswordChanged(const QString &password) {
  // Enable login button only if both fields have content
  //   m_loginButton->setEnabled(!password.isEmpty() &&
  //                             !m_usernameInput->text().isEmpty());
}
