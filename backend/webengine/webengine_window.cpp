#include "webengine_window.h"
#include <QAction>
#include <QCloseEvent>
#include <QMessageBox>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWebEngineProfile>
#include <QWebEngineSettings>

WebEngineWindow::WebEngineWindow(quint16 port, bool devMode, QWidget *parent)
    : QMainWindow(parent), m_webView(nullptr), m_progressBar(nullptr),
      m_appContext(nullptr), m_httpServer(nullptr), m_webChannel(nullptr),
      m_fileBridge(nullptr), m_isDevelopmentMode(devMode), m_port(port) {
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
  setWindowTitle("Train Simulation App");
  resize(1400, 900);

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);

  QVBoxLayout *layout = new QVBoxLayout(centralWidget);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(0);

  m_webView = new QWebEngineView(this);
  layout->addWidget(m_webView);

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

  statusBar()->showMessage("Ready");
  if (m_isDevelopmentMode) {
    QToolBar *toolbar = addToolBar("Navigation");
    QAction *reloadAction = new QAction("⟳ Reload", this);
    connect(reloadAction, &QAction::triggered, m_webView,
            &QWebEngineView::reload);
    toolbar->addAction(reloadAction);

    QAction *devToolsAction = new QAction("🔧 DevTools", this);
    connect(devToolsAction, &QAction::triggered, this, [this]() {
      if (m_webView && m_webView->page()) {
        QWebEngineView *devToolsView = new QWebEngineView(this);
        devToolsView->setAttribute(Qt::WA_DeleteOnClose, true);
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
}

void WebEngineWindow::setupWebEngine() {
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

#ifdef QT_DEBUG
  settings->setAttribute(QWebEngineSettings::JavascriptCanAccessClipboard,
                         true);
#endif

  QWebEngineProfile *profile = m_webView->page()->profile();
  profile->setHttpUserAgent("TrainSimulationApp/1.0 QtWebEngine");

  m_webChannel = new QWebChannel(this);
  m_fileBridge = new FileBridge(this);

  m_webChannel->registerObject("fileBridge", m_fileBridge);

  m_webView->page()->setWebChannel(m_webChannel);
}

void WebEngineWindow::setupConnections() {
  connect(m_webView, &QWebEngineView::loadStarted, this,
          &WebEngineWindow::onLoadStarted);
  connect(m_webView, &QWebEngineView::loadProgress, this,
          &WebEngineWindow::onLoadProgress);
  connect(m_webView, &QWebEngineView::loadFinished, this,
          &WebEngineWindow::onLoadFinished);
  connect(m_webView, &QWebEngineView::titleChanged, this,
          &WebEngineWindow::onTitleChanged);
}

void WebEngineWindow::setupBackendServer() {
  m_appContext = new AppContext();

  m_httpServer = new HttpServer(*m_appContext);

  QByteArray staticRoot = qgetenv("TRAIN_APP_STATIC_ROOT");
  if (!staticRoot.isEmpty()) {
    m_httpServer->setStaticRoot(QString::fromUtf8(staticRoot));
  }

  if (m_httpServer->startServer(m_port)) {
    statusBar()->showMessage(
        QString("Backend ready on port %1").arg(m_httpServer->getPort()));
  } else {
    QMessageBox::critical(
        this, "Server Error",
        QString("Failed to start backend server on port %1").arg(m_port));
  }
}

void WebEngineWindow::loadFrontend(const QUrl &url) {
  m_frontendUrl = url.toString();
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
  } else {
    statusBar()->showMessage("Failed to load frontend", 5000);

    QMessageBox::warning(this, "Load Error",
                         QString("Failed to load frontend from:\n%1\n\n"
                                 "Make sure the frontend server is running:\n"
                                 "  cd frontend && npm run dev")
                             .arg(m_frontendUrl));
  }
}

void WebEngineWindow::onTitleChanged(const QString &title) {
  setWindowTitle(QString("%1 - Train Simulation").arg(title));
}

void WebEngineWindow::closeEvent(QCloseEvent *event) {
  QMessageBox::StandardButton reply = QMessageBox::question(
      this, "Exit Application", "Are you sure you want to exit?",
      QMessageBox::Yes | QMessageBox::No);

  if (reply == QMessageBox::Yes) {

    if (m_httpServer) {
      m_httpServer->stopServer();
    }

    event->accept();
  } else {
    event->ignore();
  }
}
