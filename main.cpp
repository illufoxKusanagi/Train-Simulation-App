#include "core/appcontext.h"
#include "mainwindow/mainwindow.h"

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
  while (!loggedIn) {
    bool okUsername, okPassword;
    QString username =
        QInputDialog::getText(nullptr, "Login", "Username:", QLineEdit::Normal,
                              QString(), &okUsername);
    if (!okUsername) {
      qDebug() << "Login cancelled by user (username input).";
      return 0;
    }
    QString password = QInputDialog::getText(nullptr, "Login",
                                             "Password:", QLineEdit::Password,
                                             QString(), &okPassword);
    if (!okPassword) {
      qDebug() << "Login cancelled by user (password input).";
      return 0;
    }
    if (context.authManager->login(username, password)) {
      loggedIn = true;
      QMessageBox::information(nullptr, "Login Successful",
                               "Welcome to the Train Simulation App!");
    } else {
      QMessageBox::critical(nullptr, "Login Failed",
                            "Invalid username or password. Please try again.");
    }
  }
  MainWindow mainWindow(context);
  mainWindow.show();
  // QString hashed = QString(
  //     QCryptographicHash::hash("password",
  //     QCryptographicHash::Sha256).toHex());
  // qDebug() << hashed;

  return app.exec();
}
