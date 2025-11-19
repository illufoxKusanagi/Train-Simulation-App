#include "webengine_window.h"
#include <QAction>
#include <QCloseEvent>
#include <QDebug>
#include <QMessageBox>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWebEngineProfile>
#include <QWebEngineSettings>


WebEngineWindow::WebEngineWindow(QWidget *parent)
    : QMainWindow(parent), m_webView(nullptr), m_progressBar(nullptr),
      m_appContext(nullptr), m_httpServer(nullptr), m_webChannel(nullptr),
      m_fileBridge(nullptr), m_isDevelopmentMode(false) {
  setupUi();
  setupBackendServer();
  setupWebEngine();
  setupConnections();
}

WebEngineWindow::~WebEngineWindow() {
  if (m_httpServer) {
    m_httpServer->stopServer();
    delete m_httpServer;
  }
  if (m_appContext) {
    delete m_appContext;
  }
  if (m_fileBridge) {
    delete m_fileBridge;
  }
  if (m_webChannel) {
    delete m_webChannel;
  }
}

void WebEngineWindow::setupUi() {
  // Set window properties
  setWindowTitle("Train Simulation App");
  resize(1400, 900);

  // Create central widget
  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  // Create layout
  QVBoxLayout *layout = new QVBoxLayout(centralWidget);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  // Create web view
  m_webView = new QWebEngineView(this);
  layout->addWidget(m_webView);

  // Create progress bar
  m_progressBar = new QProgressBar(this);
  m_progressBar->setMaximumHeight(3);
  m_progressBar->setTextVisible(false);
  m_progressBar->setStyleSheet("QProgressBar {"
                               "    border: none;"
                               "    background-color: #e0e0e0;"
                               "}"
                               "QProgressBar::chunk {"
                               "    background-color: #2196F3;"
                               "}");
  m_progressBar->hide();
  layout->addWidget(m_progressBar);

  // Create status bar
  statusBar()->showMessage("Ready");

  // Create toolbar (optional - for dev tools, reload, etc.)
  QToolBar *toolbar = addToolBar("Navigation");

  QAction *reloadAction = new QAction("⟳ Reload", this);
  connect(reloadAction, &QAction::triggered, m_webView,
          &QWebEngineView::reload);
  toolbar->addAction(reloadAction);

  QAction *devToolsAction = new QAction("🔧 DevTools", this);
  connect(devToolsAction, &QAction::triggered, this, [this]() {
    if (m_webView && m_webView->page()) {
      // Toggle dev tools
      QWebEngineView *devToolsView = new QWebEngineView();
      m_webView->page()->setDevToolsPage(devToolsView->page());
      devToolsView->setWindowTitle("DevTools - Train Simulation");
      devToolsView->resize(1200, 800);
      devToolsView->show();
    }
  });
  toolbar->addAction(devToolsAction);

  toolbar->addSeparator();

  QAction *aboutAction = new QAction("ℹ About", this);
  connect(aboutAction, &QAction::triggered, this, [this]() {
    QMessageBox::about(this, "Train Simulation App",
                       "<h2>Train Simulation App</h2>"
                       "<p>Version 0.1.0</p>"
                       "<p>A modern train simulation application</p>"
                       "<p><b>Technology Stack:</b></p>"
                       "<ul>"
                       "<li>Qt 6 + WebEngine (Desktop Container)</li>"
                       "<li>Next.js + React (Frontend UI)</li>"
                       "<li>C++ Backend (Simulation Engine)</li>"
                       "</ul>");
  });
  toolbar->addAction(aboutAction);
}

void WebEngineWindow::setupWebEngine() {
  // Configure WebEngine settings
  QWebEngineSettings *settings = m_webView->settings();
  settings->setAttribute(QWebEngineSettings::LocalStorageEnabled, true);
  settings->setAttribute(QWebEngineSettings::LocalContentCanAccessRemoteUrls,
                         true);
  settings->setAttribute(QWebEngineSettings::AllowRunningInsecureContent,
                         false);
  settings->setAttribute(QWebEngineSettings::JavascriptEnabled, true);
  settings->setAttribute(QWebEngineSettings::JavascriptCanOpenWindows, true);
  settings->setAttribute(QWebEngineSettings::LocalContentCanAccessFileUrls,
                         true);

// Enable developer tools in debug mode
#ifdef QT_DEBUG
  settings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard,
                         true);
#endif

  // Set user agent (optional)
  QWebEngineProfile *profile = m_webView->page()->profile();
  profile->setHttpUserAgent("TrainSimulationApp/1.0 QtWebEngine");

  // Setup Qt WebChannel for native file dialogs
  m_webChannel = new QWebChannel(this);
  m_fileBridge = new FileBridge(this);

  // Register the file bridge object for JavaScript access
  m_webChannel->registerObject("fileBridge", m_fileBridge);

  // Connect the web channel to the web page
  m_webView->page()->setWebChannel(m_webChannel);

  qInfo() << "✅ Qt WebChannel setup complete - fileBridge registered";
}

void WebEngineWindow::setupConnections() {
  // Connect web view signals
  connect(m_webView, &QWebEngineView::loadStarted, this,
          &WebEngineWindow::onLoadStarted);
  connect(m_webView, &QWebEngineView::loadProgress, this,
          &WebEngineWindow::onLoadProgress);
  connect(m_webView, &QWebEngineView::loadFinished, this,
          &WebEngineWindow::onLoadFinished);
  connect(m_webView, &QWebEngineView::urlChanged, this,
          &WebEngineWindow::onUrlChanged);
  connect(m_webView, &QWebEngineView::titleChanged, this,
          &WebEngineWindow::onTitleChanged);
}

void WebEngineWindow::setupBackendServer() {
  // Initialize app context
  m_appContext = new AppContext();

  // Create and start HTTP server
  m_httpServer = new HttpServer(*m_appContext);

  quint16 port = 8080;
  if (m_httpServer->startServer(port)) {
    qInfo() << "✅ Backend server started on port" << port;
    statusBar()->showMessage(QString("Backend ready on port %1").arg(port));
  } else {
    qCritical() << "❌ Failed to start backend server on port" << port;
    QMessageBox::critical(
        this, "Server Error",
        QString("Failed to start backend server on port %1").arg(port));
  }
}

void WebEngineWindow::loadFrontend(const QUrl &url) {
  m_frontendUrl = url.toString();
  qInfo() << "Loading frontend from:" << url;
  m_webView->load(url);
}

void WebEngineWindow::onLoadStarted() {
  m_progressBar->setValue(0);
  m_progressBar->show();
  statusBar()->showMessage("Loading...");
}

void WebEngineWindow::onLoadProgress(int progress) {
  m_progressBar->setValue(progress);
}

void WebEngineWindow::onLoadFinished(bool ok) {
  m_progressBar->hide();

  if (ok) {
    statusBar()->showMessage("Ready", 3000);
    qInfo() << "✅ Frontend loaded successfully";
  } else {
    statusBar()->showMessage("Failed to load frontend", 5000);
    qWarning() << "❌ Failed to load frontend from:" << m_frontendUrl;

    QMessageBox::warning(this, "Load Error",
                         QString("Failed to load frontend from:\n%1\n\n"
                                 "Make sure the frontend server is running:\n"
                                 "  cd frontend && npm run dev")
                             .arg(m_frontendUrl));
  }
}

void WebEngineWindow::onUrlChanged(const QUrl &url) {
  qDebug() << "URL changed:" << url;
}

void WebEngineWindow::onTitleChanged(const QString &title) {
  setWindowTitle(QString("%1 - Train Simulation").arg(title));
}

void WebEngineWindow::closeEvent(QCloseEvent *event) {
  QMessageBox::StandardButton reply = QMessageBox::question(
      this, "Exit Application", "Are you sure you want to exit?",
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {
    qInfo() << "Application closing...";

    // Stop backend server
    if (m_httpServer) {
      m_httpServer->stopServer();
    }

    event->accept();
  } else {
    event->ignore();
  }
}
