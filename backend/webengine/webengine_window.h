#ifndef WEBENGINE_WINDOW_H
#define WEBENGINE_WINDOW_H

#include "core/appcontext.h"
#include "file_bridge.h"
#include "http_server/http_server.h"
#include <QMainWindow>
#include <QProgressBar>
#include <QStatusBar>
#include <QWebChannel>
#include <QWebEnginePage>
#include <QWebEngineView>
#include <QtGlobal>

/**
 * @brief Main window that embeds Next.js frontend using Qt WebEngine
 *
 * This window provides a desktop container for the web-based UI.
 * It can load the frontend from:
 * 1. Development server (http://localhost:3000) - for hot reload
 * 2. Production build (static files) - for release
 * 3. Embedded resources - for standalone distribution
 */
class WebEngineWindow : public QMainWindow {
  Q_OBJECT

public:
  /**
   * @param port  The TCP port the backend HTTP server should bind to.
   *              Pass 0 to let the OS pick a free port.
   * @param parent Parent widget (usually nullptr for a top-level window).
   */
  explicit WebEngineWindow(quint16 port = 8080, bool devMode = false,
                           QWidget *parent = nullptr);
  ~WebEngineWindow();

  /**
   * @brief Load the frontend URL
   * @param url URL to load (e.g., "http://localhost:3000" or
   * "qrc:/frontend/index.html")
   */
  void loadFrontend(const QUrl &url);

  /**
   * @brief Get the backend HTTP server instance
   */
  HttpServer *getHttpServer() { return m_httpServer; }

protected:
  void closeEvent(QCloseEvent *event) override;

private slots:
  void onLoadStarted();
  void onLoadProgress(int progress);
  void onLoadFinished(bool ok);
  void onTitleChanged(const QString &title);

private:
  void setupUi();
  void setupWebEngine();
  void setupConnections();
  void setupBackendServer();

  // UI Components
  QWebEngineView *m_webView;
  QProgressBar *m_progressBar;

  // Backend
  AppContext *m_appContext;
  HttpServer *m_httpServer;

  // WebChannel integration
  QWebChannel *m_webChannel;
  FileBridge *m_fileBridge;

  // Settings
  bool m_isDevelopmentMode;
  QString m_frontendUrl;
  quint16 m_port;
};

#endif // WEBENGINE_WINDOW_H
