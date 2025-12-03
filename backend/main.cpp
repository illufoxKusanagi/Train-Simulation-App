#include "core/appcontext.h"
#include "http_server/http_server.h"
#include "webengine/webengine_window.h"
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QUrl>

int main(int argc, char *argv[]) {
  // Parse command line arguments first
  bool headless = false;
  bool devMode = false;
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

  // Auto-detect frontend URL if not specified
  if (frontendUrl.isEmpty()) {
    if (devMode) {
      // In dev mode, use the Next.js dev server
      frontendUrl = "http://127.0.0.1:3254";
    } else {
      // In production mode, backend serves the static frontend
      frontendUrl = QString("http://127.0.0.1:%1").arg(port);
    }
  }

  if (headless) {
    // Headless mode: Backend server only (no GUI)
    QCoreApplication app(argc, argv);
    QCoreApplication::setOrganizationName("PT INKA Persero");
    QCoreApplication::setApplicationName("Train Simulation App");

    AppContext context;
    HttpServer server(context);

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

    // Start the backend server first
    AppContext context;
    HttpServer server(context);

    if (!server.startServer(port)) {
      qCritical() << "❌ Failed to start backend server on port" << port;
      return 1;
    }

    qInfo() << "🚀 Starting Train Simulation App (Desktop Mode)";
    qInfo() << "   Mode:" << (devMode ? "Development" : "Production");
    qInfo() << "   Backend Port:" << server.getPort();
    qInfo() << "   Frontend:" << frontendUrl;

    // Create main window with embedded web view
    WebEngineWindow window;
    window.show();

    // Give the server a moment to fully start, then load frontend
    QTimer::singleShot(100, [&window, frontendUrl]() {
      window.loadFrontend(QUrl(frontendUrl));
    });

    qInfo() << "✅ Application started successfully";
    qInfo() << "💡 Usage:";
    qInfo() << "   --headless        Run backend server only (no GUI)";
    qInfo() << "   --dev             Development mode";
    qInfo() << "   --port=8080       Set backend port";
    qInfo() << "   --frontend=URL    Set frontend URL";

    return app.exec();
  }
}