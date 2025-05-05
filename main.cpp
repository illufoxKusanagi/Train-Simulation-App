#include "core/appcontext.h"
#include "mainwindow/mainwindow.h"
#include "widgets/login_dialog_widget.h"

#include <QApplication>
#include <QDebug>
#include <QInputDialog>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("PT INKA Persero");
  QCoreApplication::setApplicationName("Train Simulation App");
  app.setStyleSheet("QWidget { background-color: white; color: black; }");
  AppContext context;
  bool loggedIn = false;
  LoginDialogWidget loginDialog;
  // Connect login attempt signal to authentication handler
  QObject::connect(
      &loginDialog, &LoginDialogWidget::loginAttempt,
      [&context, &loggedIn, &loginDialog](const QString &username,
                                          const QString &password) {
        if (context.authManager->login(username, password)) {
          // Authentication successful
          loggedIn = true;
          QMessageBox::information(nullptr, "Login Successful",
                                   "Welcome to the Train Simulation App!");
          loginDialog.accept(); // Close dialog on success
        } else {
          // Authentication failed
          QMessageBox::critical(
              nullptr, "Login Failed",
              "Invalid username or password. Please try again.");
          // Leave dialog open for another attempt
        }
      });

  // Show the login dialog
  int result = loginDialog.exec();

  // If login was successful, show the main window
  if (result == QDialog::Accepted && loggedIn) {
    MainWindow mainWindow(context);
    mainWindow.show();
    return app.exec();
  } else {
    // User cancelled or closed the login dialog
    qDebug() << "Login cancelled by user.";
    return 0;
  }
  // while (!loggedIn) {
  //   bool okUsername, okPassword;
  //   QString username =
  //       QInputDialog::getText(nullptr, "Login", "Username:",
  //       QLineEdit::Normal,
  //                             QString(), &okUsername);
  //   if (!okUsername) {
  //     qDebug() << "Login cancelled by user (username input).";
  //     return 0;
  //   }
  //   QString password = QInputDialog::getText(nullptr, "Login",
  //                                            "Password:",
  //                                            QLineEdit::Password, QString(),
  //                                            &okPassword);
  //   if (!okPassword) {
  //     qDebug() << "Login cancelled by user (password input).";
  //     return 0;
  //   }
  //   if (context.authManager->login(username, password)) {
  //     loggedIn = true;
  //     QMessageBox::information(nullptr, "Login Successful",
  //                              "Welcome to the Train Simulation App!");
  //   } else {
  //     QMessageBox::critical(nullptr, "Login Failed",
  //                           "Invalid username or password. Please try
  //                           again.");
  //   }
  // }
  MainWindow mainWindow(context);
  mainWindow.show();
  // QString hashed = QString(
  //     QCryptographicHash::hash("password",
  //     QCryptographicHash::Sha256).toHex());
  // qDebug() << hashed;

  return app.exec();
}
