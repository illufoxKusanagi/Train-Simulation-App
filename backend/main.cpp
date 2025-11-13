#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>
#include <QUrl>
#include "core/appcontext.h"
#include "http_server/http_server.h"
#include "webengine/webengine_window.h"

int main(int argc, char *argv[])
{
    // Parse command line arguments first
    bool headless = false;
    bool devMode = false;
    quint16 port = 8080;
    QString frontendUrl = "http://localhost:3000"; // Default dev server

    for (int i = 1; i < argc; i++)
    {
        QString arg(argv[i]);
        if (arg == "--headless")
        {
            headless = true;
        }
        else if (arg == "--dev")
        {
            devMode = true;
        }
        else if (arg.startsWith("--port="))
        {
            port = arg.mid(7).toInt();
        }
        else if (arg.startsWith("--frontend="))
        {
            frontendUrl = arg.mid(11);
        }
    }

    if (headless)
    {
        // Headless mode: Backend server only (no GUI)
        QCoreApplication app(argc, argv);
        QCoreApplication::setOrganizationName("PT INKA Persero");
        QCoreApplication::setApplicationName("Train Simulation App");

        AppContext context;
        HttpServer server(context);

        if (server.startServer(port))
        {
            qInfo() << "✅ Train Simulation Backend Server started on port" << port;
            qInfo() << "Server started successfully on port:" << server.getPort();
            qInfo() << "Available endpoints:";
            qInfo() << "  GET  /status - Server status";
            qInfo() << "  GET  /api/health - Health check";
            qInfo() << "  GET  /api/parameters/train - Get train parameters";
            qInfo() << "  POST /api/parameters/train - Update train parameters";
            qInfo() << "  GET  /api/parameters/electrical - Get electrical parameters";
            qInfo() << "  POST /api/parameters/electrical - Update electrical parameters";
            qInfo() << "  GET  /api/parameters/running - Get running parameters";
            qInfo() << "  POST /api/parameters/running - Update running parameters";
            qInfo() << "  GET  /api/parameters/track - Get track parameters";
            qInfo() << "  POST /api/parameters/track - Update track parameters";
            qInfo() << "  POST /api/simulation/start - Start simulation";
            qInfo() << "  GET  /api/simulation/status - Get simulation status";
            qInfo() << "  GET  /api/simulation/results - Get simulation results";
            qInfo() << "  POST /api/export/results - Export results to CSV";
            return app.exec();
        }
        else
        {
            qCritical() << "❌ Failed to start server on port" << port;
            return 1;
        }
    }
    else
    {
        // GUI mode: Qt WebEngine with embedded Next.js frontend
        QApplication app(argc, argv);
        QCoreApplication::setOrganizationName("PT INKA Persero");
        QCoreApplication::setApplicationName("Train Simulation App");

        qInfo() << "🚀 Starting Train Simulation App (Desktop Mode)";
        qInfo() << "   Mode:" << (devMode ? "Development" : "Production");
        qInfo() << "   Frontend:" << frontendUrl;

        // Create main window with embedded web view
        WebEngineWindow window;
        window.show();

        // Load frontend
        window.loadFrontend(QUrl(frontendUrl));

        qInfo() << "✅ Application started successfully";
        qInfo() << "💡 Usage:";
        qInfo() << "   --headless        Run backend server only (no GUI)";
        qInfo() << "   --dev             Development mode";
        qInfo() << "   --port=8080       Set backend port";
        qInfo() << "   --frontend=URL    Set frontend URL (default: http://localhost:3000)";

        return app.exec();
    }
}

// #include <QCoreApplication>  // Changed from QApplication
// #include <QDebug>
// #include <QTimer>
// #include "core/appcontext.h"
// #include "http_server/http_server.h"

// int main(int argc, char *argv[]) {
//     QCoreApplication app(argc, argv);  // No GUI needed

//     // Parse command line arguments
//     bool headless = false;
//     int port = 8080;

//     for (int i = 1; i < argc; i++) {
//         if (QString(argv[i]) == "--server") {
//             headless = true;
//         } else if (QString(argv[i]) == "--port" && i + 1 < argc) {
//             port = QString(argv[i + 1]).toInt();
//             i++; // Skip next argument
//         }
//     }

//     if (!headless) {
//         qDebug() << "Usage: TrainSimulationApp --server [--port 8080]";
//         return 1;
//     }

//     // Initialize application context
//     AppContext context;

//     // Start HTTP server
//     HttpServer server(context);
//     if (!server.startServer(port)) {
//         qDebug() << "Failed to start server. Exiting.";
//         return 1;
//     }

//     qDebug() << "Train Simulation Backend is running...";
//     qDebug() << "Press Ctrl+C to stop";

//     return app.exec();
// }

// // #include "core/appcontext.h"
// // #include "mainwindow/mainwindow.h"
// // #include "widgets/login_dialog_widget.h"
// // #include "widgets/message_box_widget.h"
// // #include <QApplication>
// // #include <QDebug>
// // #include <QIcon>
// // #include <QInputDialog>
// // #include <QSize>

// // int main(int argc, char *argv[]) {
// //   QApplication app(argc, argv);
// //   QCoreApplication::setOrganizationName("PT INKA Persero");
// //   QCoreApplication::setApplicationName("Train Simulation App");
// //   QIcon appIcon;
// //   appIcon.addFile(":/icons/icons/trainSimulationAppLogo.png", QSize(64, 64));
// //   app.setWindowIcon(appIcon);
// //   app.setStyleSheet("QWidget { background-color: white; color: black; }");
// //   AppContext context;
// //   bool loggedIn = false;
// //   LoginDialogWidget loginDialog;
// //   QObject::connect(
// //       &loginDialog, &LoginDialogWidget::loginAttempt,
// //       [&context, &loggedIn, &loginDialog](const QString &username,
// //                                           const QString &password) {
// //         if (context.authManager->login(username, password)) {
// //           loggedIn = true;
// //           MessageBoxWidget messageBox("Login Successful",
// //                                       "Welcome to the Train Simulation App!",
// //                                       MessageBoxWidget::Information);
// //           loginDialog.accept();
// //         } else {
// //           MessageBoxWidget messageBox(
// //               "Login Failed", "Invalid username or password. Please try again.",
// //               MessageBoxWidget::Critical);
// //         }
// //       });
// //   int result = loginDialog.exec();
// //   if (result == QDialog::Accepted && loggedIn) {
// //     MainWindow mainWindow(context);
// //     mainWindow.show();
// //     return app.exec();
// //   } else {
// //     return 0;
// //   }
// // }
