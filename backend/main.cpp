#include "core/appcontext.h"
#include "http_server/http_server.h"
#include "webengine/webengine_window.h"
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QUrl>

static void silentMessageHandler(QtMsgType, const QMessageLogContext &,
                                 const QString &) {
  // Production: discard all Qt log output (no console window side-effects)
}

int main(int argc, char *argv[]) {
  // Parse command line arguments first
  bool headless = false;
  // Automatically enable dev mode in Debug builds; Release builds = production
#ifdef NDEBUG
  bool devMode = false;
#else
  bool devMode = true;
#endif
  quint16 port = 8080;
  QString frontendUrl = "";

  for (int i = 1; i < argc; i++) {
    QString arg(argv[i]);
    if (arg == "--headless") {
      headless = true;
    } else if (arg == "--dev") {
      devMode = true;
    } else if (arg.startsWith("--port=")) {
      port = arg.mid(7).toInt();
    } else if (arg.startsWith("--frontend=")) {
      frontendUrl = arg.mid(11);
    }
  }

  if (!devMode) {
    qInstallMessageHandler(silentMessageHandler);
  }

  if (headless) {
    // Headless mode: Backend server only (no GUI)
    QCoreApplication app(argc, argv);
    QCoreApplication::setOrganizationName("PT INKA Persero");
    QCoreApplication::setApplicationName("Train Simulation App");

    // Auto-detect frontend static root (now AFTER QCoreApplication is created)
    if (frontendUrl.isEmpty()) {
      QStringList possiblePaths = {
          QCoreApplication::applicationDirPath() + "/frontend",
          "/usr/share/train-simulation-app",
          QCoreApplication::applicationDirPath() + "/../../frontend/out"};
      for (const QString &path : possiblePaths) {
        if (QDir(path).exists() && QFile::exists(path + "/index.html")) {
          qputenv("TRAIN_APP_STATIC_ROOT", path.toUtf8());
          break;
        }
      }
    }

    AppContext context;
    HttpServer server(context);

    // Check if we need to serve static files (though headless usually doesn't
    // need frontend)
    QByteArray staticRoot = qgetenv("TRAIN_APP_STATIC_ROOT");
    if (!staticRoot.isEmpty()) {
      server.setStaticRoot(QString::fromUtf8(staticRoot));
    }

    if (server.startServer(port)) {
      qInfo() << "✅ Train Simulation Backend Server started on port" << port;
      qInfo() << "Server started successfully on port:" << server.getPort();
      qInfo() << "Available endpoints:";
      qInfo() << "  GET  /status - Server status";
      qInfo() << "  GET  /api/health - Health check";
      qInfo() << "  GET  /api/parameters/train - Get train parameters";
      qInfo() << "  POST /api/parameters/train - Update train parameters";
      qInfo()
          << "  GET  /api/parameters/electrical - Get electrical parameters";
      qInfo()
          << "  POST /api/parameters/electrical - Update electrical parameters";
      qInfo() << "  GET  /api/parameters/running - Get running parameters";
      qInfo() << "  POST /api/parameters/running - Update running parameters";
      qInfo() << "  GET  /api/parameters/track - Get track parameters";
      qInfo() << "  POST /api/parameters/track - Update track parameters";
      qInfo() << "  POST /api/simulation/start - Start simulation";
      qInfo() << "  GET  /api/simulation/status - Get simulation status";
      qInfo() << "  GET  /api/simulation/results - Get simulation results";
      qInfo() << "  POST /api/export/results - Export results to CSV";
      return app.exec();
    } else {
      qCritical() << "❌ Failed to start server on port" << port;
      return 1;
    }
  } else {
    // GUI mode: Qt WebEngine with embedded Next.js frontend
    QApplication app(argc, argv);
    app.setApplicationName("Train Simulation App");
    app.setOrganizationName("PT INKA Persero");

    // Auto-detect frontend URL now that QApplication exists and
    // applicationDirPath() is valid.
    if (frontendUrl.isEmpty()) {
      if (devMode) {
        frontendUrl = "http://127.0.0.1:3254";
      } else {
        QStringList possiblePaths = {
            // Relative to the installed/built executable
            QCoreApplication::applicationDirPath() + "/frontend",
            // Linux system-wide install (RPM/DEB)
            "/usr/share/train-simulation-app",
            // Development build fallback
            QCoreApplication::applicationDirPath() + "/../../frontend/out"};

        QString foundPath;
        for (const QString &path : possiblePaths) {
          if (QDir(path).exists() && QFile::exists(path + "/index.html")) {
            foundPath = path;
            qInfo() << "✅ Found local frontend directory at:" << path;
            break;
          }
        }

        if (!foundPath.isEmpty()) {
          // Store the path so the server can serve the static files
          qputenv("TRAIN_APP_STATIC_ROOT", foundPath.toUtf8());
          // Use a placeholder; replaced below once we know the server port
          frontendUrl = "SERVE_STATIC";
        } else {
          qWarning() << "⚠️ Could not find local frontend directory. "
                        "Defaulting to localhost.";
          frontendUrl = "http://127.0.0.1:3254";
        }
      }
    }

    qInfo() << "🚀 Starting Train Simulation App (Desktop Mode)";

    // WebEngineWindow owns AppContext, HttpServer, and WebChannel.
    // Do NOT create a second HttpServer here — that would attempt to bind the
    // same port and fail with "address already in use".
    WebEngineWindow window(port, devMode);
    window.show();

    // Now that the server is running inside the window, resolve the URL.
    if (frontendUrl == "SERVE_STATIC") {
      quint16 actualPort = window.getHttpServer()->getPort();
      frontendUrl = QString("http://127.0.0.1:%1").arg(actualPort);
    }

    qInfo() << "   Mode:" << (devMode ? "Development" : "Production");
    qInfo() << "   Frontend:" << frontendUrl;

    // Load frontend
    window.loadFrontend(QUrl(frontendUrl));

    qInfo() << "✅ Application started successfully";
    qInfo() << "💡 Usage:";
    qInfo() << "   --headless        Run backend server only (no GUI)";
    qInfo() << "   --dev             Development mode";
    qInfo() << "   --port=8080       Set backend port";
    qInfo() << "   --frontend=URL    Set frontend URL (default: "
               "http://localhost:3254)";

    return app.exec();
  }
}
