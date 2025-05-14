#include "core/appcontext.h"
#include "mainwindow/mainwindow.h"
#include "widgets/login_dialog_widget.h"
#include "widgets/message_box_widget.h"
#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QInputDialog>
#include <QSize>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  QCoreApplication::setOrganizationName("PT INKA Persero");
  QCoreApplication::setApplicationName("Train Simulation App");
  QIcon appIcon;
  appIcon.addFile(":/icons/icons/trainSimulationAppLogo.png", QSize(64, 64));
  app.setWindowIcon(appIcon);
  app.setStyleSheet("QWidget { background-color: white; color: black; }");
  AppContext context;
  // bool loggedIn = false;
  // LoginDialogWidget loginDialog;
  // QObject::connect(
  //     &loginDialog, &LoginDialogWidget::loginAttempt,
  //     [&context, &loggedIn, &loginDialog](const QString &username,
  //                                         const QString &password) {
  //       if (context.authManager->login(username, password)) {
  //         loggedIn = true;
  //         MessageBoxWidget messageBox("Login Successful",
  //                                     "Welcome to the Train Simulation App!",
  //                                     MessageBoxWidget::Information);
  //         loginDialog.accept();
  //       } else {
  //         MessageBoxWidget messageBox(
  //             "Login Failed", "Invalid username or password. Please try
  //             again.", MessageBoxWidget::Critical);
  //       }
  //     });
  // int result = loginDialog.exec();
  // if (result == QDialog::Accepted && loggedIn) {
  MainWindow mainWindow(context);
  mainWindow.show();
  return app.exec();
  // } else {
  //   return 0;
  // }
}
