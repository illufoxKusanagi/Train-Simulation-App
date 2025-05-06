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
  QObject::connect(
      &loginDialog, &LoginDialogWidget::loginAttempt,
      [&context, &loggedIn, &loginDialog](const QString &username,
                                          const QString &password) {
        if (context.authManager->login(username, password)) {
          loggedIn = true;
          QMessageBox::information(nullptr, "Login Successful",
                                   "Welcome to the Train Simulation App!");
          loginDialog.accept();
        } else {
          QMessageBox::critical(
              nullptr, "Login Failed",
              "Invalid username or password. Please try again.");
        }
      });
  int result = loginDialog.exec();
  if (result == QDialog::Accepted && loggedIn) {
    MainWindow mainWindow(context);
    mainWindow.show();
    return app.exec();
  } else {
    return 0;
  }
}
