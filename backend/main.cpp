#include "core/appcontext.h"
#include "http_server/http_server.h"
#include "webengine/webengine_window.h"
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTimer>
#include <QUrl>

static void silentMessageHandler(QtMsgType, const QMessageLogContext &,
                                 const QString &) {}

int main(int argc, char *argv[]) {
  bool headless = false;
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
    QByteArray staticRoot = qgetenv("TRAIN_APP_STATIC_ROOT");

    if (!staticRoot.isEmpty()) {
      server.setStaticRoot(QString::fromUtf8(staticRoot));
    }

    if (server.startServer(port)) {

      return app.exec();
    } else {
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

            break;
          }
        }

        if (!foundPath.isEmpty()) {
          qputenv("TRAIN_APP_STATIC_ROOT", foundPath.toUtf8());
          frontendUrl = "SERVE_STATIC";
        } else {
          frontendUrl = "http://127.0.0.1:3254";
        }
      }
    }

    WebEngineWindow window(port, devMode);
    window.show();

    if (frontendUrl == "SERVE_STATIC") {
      quint16 actualPort = window.getHttpServer()->getPort();
      frontendUrl = QString("http://127.0.0.1:%1").arg(actualPort);
    }

    window.loadFrontend(QUrl(frontendUrl));
    return app.exec();
  }
}
