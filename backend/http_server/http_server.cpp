#include "http_server.h"
#include <QCoreApplication>
#include <QDebug>
#include <QHostAddress>
#include <QHttpHeaders>
#include <QtGlobal>

#include <QHttpServerRequest>
#include <QHttpServerResponder>
#include <QHttpServerResponse>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QPair>

HttpServer::HttpServer(AppContext &context, QObject *parent)
    : QObject(parent), m_context(context), m_port(0) {
  m_httpServer = new QHttpServer(this);
  m_apiHandler = new ApiHandler(context, this);
  setupRoutes();
}

bool HttpServer::startServer(quint16 port) {
  m_tcpServer = std::make_unique<QTcpServer>();

  if (port == 0) {
    if (!m_tcpServer->listen(QHostAddress::LocalHost)) {
      qCritical() << "Failed to listen on localhost:"
                  << m_tcpServer->errorString();
      return false;
    }
  } else {
    if (!m_tcpServer->listen(QHostAddress::LocalHost, port)) {
      qCritical() << "Failed to listen on port" << port << ":"
                  << m_tcpServer->errorString();
      return false;
    }
  }

  m_httpServer->bind(m_tcpServer.get());

  m_port = m_tcpServer->serverPort();
  qInfo()
      << "🚀 Train Simulation Backend Server is running on http://localhost:"
      << m_port;

  return true;
}

void HttpServer::stopServer() {
  if (m_httpServer) {
    qInfo() << "🛑 Stopping HTTP server...";
  }
}

quint16 HttpServer::getPort() const { return m_port; }

QJsonObject HttpServer::parseRequestBody(const QHttpServerRequest &request) {
  QByteArray body = request.body();
  QJsonParseError error;
  QJsonDocument doc = QJsonDocument::fromJson(body, &error);

  if (error.error != QJsonParseError::NoError) {
    qWarning() << "JSON parse error:" << error.errorString();
    return QJsonObject();
  }
  return doc.object();
}

void HttpServer::setupRoutes() {
  // Helper function to add CORS headers to any response
  auto addCorsHeaders = [](QHttpServerResponse &&response) {
#if QT_VERSION < QT_VERSION_CHECK(6, 8, 0)
    QHttpServerResponder::HeaderList headers = {
        {QByteArrayLiteral("Access-Control-Allow-Origin"),
         QByteArrayLiteral("*")},
        {QByteArrayLiteral("Access-Control-Allow-Methods"),
         QByteArrayLiteral("GET, POST, PUT, DELETE, OPTIONS")},
        {QByteArrayLiteral("Access-Control-Allow-Headers"),
         QByteArrayLiteral("Content-Type, Authorization")},
        {QByteArrayLiteral("Access-Control-Max-Age"),
         QByteArrayLiteral("86400")},
        // For Qt 6.7, setHeaders might overwrite existing headers (like
        // Content-Type from constructor). We re-add it just in case, assuming
        // JSON for most responses.
        {QByteArrayLiteral("Content-Type"),
         QByteArrayLiteral("application/json")}};
    response.setHeaders(headers);
#else
    QHttpHeaders headers = response.headers();
    headers.append("Access-Control-Allow-Origin", "*");
    headers.append("Access-Control-Allow-Methods",
                   "GET, POST, PUT, DELETE, OPTIONS");
    headers.append("Access-Control-Allow-Headers",
                   "Content-Type, Authorization");
    headers.append("Access-Control-Max-Age", "86400");
    response.setHeaders(headers);
#endif
    return std::move(response);
  };

  // Handle OPTIONS preflight requests
  m_httpServer->route("/api/health", QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "📋 OPTIONS /api/health";
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::Ok));
                      });

  // Handle OPTIONS for /api/parameters/train
  m_httpServer->route("/api/parameters/train",
                      QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "📋 OPTIONS /api/parameters/train";
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::Ok));
                      });

  // Handle OPTIONS for /api/parameters/electrical
  m_httpServer->route("/api/parameters/electrical",
                      QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "📋 OPTIONS /api/parameters/electrical";
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::Ok));
                      });

  // Handle OPTIONS for /api/parameters/running
  m_httpServer->route("/api/parameters/running",
                      QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "📋 OPTIONS /api/parameters/running";
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::Ok));
                      });

  // Handle OPTIONS for /api/parameters/track
  m_httpServer->route("/api/parameters/track",
                      QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "📋 OPTIONS /api/parameters/track";
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::Ok));
                      });

  // Handle OPTIONS for /api/simulation/start
  m_httpServer->route("/api/simulation/start",
                      QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "📋 OPTIONS /api/simulation/start";
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::Ok));
                      });

  // Handle OPTIONS for /api/simulation/status
  m_httpServer->route("/api/simulation/status",
                      QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "📋 OPTIONS /api/simulation/status";
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::Ok));
                      });

  // Handle OPTIONS for /api/simulation/results
  m_httpServer->route("/api/simulation/results",
                      QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "📋 OPTIONS /api/simulation/results";
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::Ok));
                      });

  // Handle OPTIONS for /api/export/results
  m_httpServer->route("/api/export/results",
                      QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "📋 OPTIONS /api/export/results";
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::Ok));
                      });

  // Handle OPTIONS for /status
  m_httpServer->route("/status", QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "📋 OPTIONS /status";
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::Ok));
                      });

  // Health check endpoint
  m_httpServer->route("/api/health", QHttpServerRequest::Method::Get,
                      [this, addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "🔍 GET /api/health";
                        return addCorsHeaders(
                            m_apiHandler->handleHealthCheck());
                      });

  // Quick initialization endpoint
  m_httpServer->route("/api/init/quick", QHttpServerRequest::Method::Post,
                      [this, addCorsHeaders](const QHttpServerRequest &) {
                        qDebug()
                            << "🔧 POST /api/init/quick - Quick initialization";
                        return addCorsHeaders(m_apiHandler->handleQuickInit());
                      });

  m_httpServer->route("/api/init/quick", QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::Ok));
                      });

  // Debug endpoint
  m_httpServer->route(
      "/api/debug/context", QHttpServerRequest::Method::Get,
      [this, addCorsHeaders](const QHttpServerRequest &) {
        qDebug() << "🐛 GET /api/debug/context - Checking AppContext values";
        return addCorsHeaders(m_apiHandler->handleDebugContext());
      });

  m_httpServer->route("/api/debug/context", QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::Ok));
                      });

  // Train parameters endpoints
  m_httpServer->route("/api/parameters/train", QHttpServerRequest::Method::Get,
                      [this, addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "🔍 GET /api/parameters/train";
                        return addCorsHeaders(
                            m_apiHandler->handleGetTrainParameters());
                      });

  m_httpServer->route(
      "/api/parameters/train", QHttpServerRequest::Method::Post,
      [this, addCorsHeaders](const QHttpServerRequest &request) {
        qDebug() << "📝 POST /api/parameters/train";
        qDebug() << "📦 Request body:" << request.body();
        QJsonObject data = parseRequestBody(request);
        qDebug() << "🔧 Parsed JSON:" << data;
        return addCorsHeaders(m_apiHandler->handleUpdateTrainParameters(data));
      });

  m_httpServer->route(
      "/api/parameters/electrical", QHttpServerRequest::Method::Get,
      [this, addCorsHeaders](const QHttpServerRequest &) {
        qDebug() << "🔍 GET /api/parameters/electrical";
        return addCorsHeaders(m_apiHandler->handleGetElectricalParameters());
      });

  m_httpServer->route(
      "/api/parameters/electrical", QHttpServerRequest::Method::Post,
      [this, addCorsHeaders](const QHttpServerRequest &request) {
        qDebug() << "📝 POST /api/parameters/electrical";
        QJsonObject data = parseRequestBody(request);
        return addCorsHeaders(
            m_apiHandler->handleUpdateElectricalParameters(data));
      });

  // Running parameters endpoints
  m_httpServer->route(
      "/api/parameters/running", QHttpServerRequest::Method::Get,
      [this, addCorsHeaders](const QHttpServerRequest &) {
        qDebug() << "🔍 GET /api/parameters/running";
        return addCorsHeaders(m_apiHandler->handleGetRunningParameters());
      });

  m_httpServer->route(
      "/api/parameters/running", QHttpServerRequest::Method::Post,
      [this, addCorsHeaders](const QHttpServerRequest &request) {
        qDebug() << "📝 POST /api/parameters/running";
        QJsonObject data = parseRequestBody(request);
        return addCorsHeaders(
            m_apiHandler->handleUpdateRunningParameters(data));
      });

  // Track parameters endpoints
  m_httpServer->route("/api/parameters/track", QHttpServerRequest::Method::Get,
                      [this, addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "🔍 GET /api/parameters/track";
                        return addCorsHeaders(
                            m_apiHandler->handleGetTrackParameters());
                      });

  m_httpServer->route(
      "/api/parameters/track", QHttpServerRequest::Method::Post,
      [this, addCorsHeaders](const QHttpServerRequest &request) {
        qDebug() << "📝 POST /api/parameters/track";
        QJsonObject data = parseRequestBody(request);
        return addCorsHeaders(m_apiHandler->handleUpdateTrackParameters(data));
      });

  // Mass parameters
  m_httpServer->route("/api/parameters/mass",
                      QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::NoContent));
                      });

  m_httpServer->route("/api/parameters/mass", QHttpServerRequest::Method::Get,
                      [this, addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "🔍 GET /api/parameters/mass";
                        return addCorsHeaders(
                            m_apiHandler->handleGetMassParameters());
                      });

  m_httpServer->route(
      "/api/parameters/mass", QHttpServerRequest::Method::Post,
      [this, addCorsHeaders](const QHttpServerRequest &request) {
        qDebug() << "📝 POST /api/parameters/mass";
        QJsonObject data = parseRequestBody(request);
        return addCorsHeaders(m_apiHandler->handleUpdateMassParameters(data));
      });

  // Calculate mass endpoint
  m_httpServer->route("/api/calculate/mass",
                      QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "📋 OPTIONS /api/calculate/mass";
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::Ok));
                      });

  m_httpServer->route(
      "/api/calculate/mass", QHttpServerRequest::Method::Post,
      [this, addCorsHeaders](const QHttpServerRequest &request) {
        qDebug() << "🧮 POST /api/calculate/mass";
        QJsonObject data = parseRequestBody(request);
        return addCorsHeaders(m_apiHandler->handleCalculateMass(data));
      });

  // Car number parameters
  m_httpServer->route("/api/parameters/carnumber",
                      QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::NoContent));
                      });

  m_httpServer->route(
      "/api/parameters/carnumber", QHttpServerRequest::Method::Get,
      [this, addCorsHeaders](const QHttpServerRequest &) {
        qDebug() << "🔍 GET /api/parameters/carnumber";
        return addCorsHeaders(m_apiHandler->handleGetCarNumberParameters());
      });

  m_httpServer->route(
      "/api/parameters/carnumber", QHttpServerRequest::Method::Post,
      [this, addCorsHeaders](const QHttpServerRequest &request) {
        qDebug() << "📝 POST /api/parameters/carnumber";
        QJsonObject data = parseRequestBody(request);
        return addCorsHeaders(
            m_apiHandler->handleUpdateCarNumberParameters(data));
      });

  // Passenger parameters
  m_httpServer->route("/api/parameters/passenger",
                      QHttpServerRequest::Method::Options,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        return addCorsHeaders(QHttpServerResponse(
                            QHttpServerResponse::StatusCode::NoContent));
                      });

  m_httpServer->route(
      "/api/parameters/passenger", QHttpServerRequest::Method::Get,
      [this, addCorsHeaders](const QHttpServerRequest &) {
        qDebug() << "🔍 GET /api/parameters/passenger";
        return addCorsHeaders(m_apiHandler->handleGetPassengerParameters());
      });

  m_httpServer->route(
      "/api/parameters/passenger", QHttpServerRequest::Method::Post,
      [this, addCorsHeaders](const QHttpServerRequest &request) {
        qDebug() << "📝 POST /api/parameters/passenger";
        QJsonObject data = parseRequestBody(request);
        return addCorsHeaders(
            m_apiHandler->handleUpdatePassengerParameters(data));
      });

  // Simulation control endpoints
  m_httpServer->route(
      "/api/simulation/start", QHttpServerRequest::Method::Post,
      [this, addCorsHeaders](const QHttpServerRequest &request) {
        qDebug() << "🚀 POST /api/simulation/start";
        QJsonObject data = parseRequestBody(request);
        return addCorsHeaders(m_apiHandler->handleStartSimulation(data));
      });

  m_httpServer->route("/api/simulation/status", QHttpServerRequest::Method::Get,
                      [this, addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "📊 GET /api/simulation/status";
                        return addCorsHeaders(
                            m_apiHandler->handleGetSimulationStatus());
                      });

  m_httpServer->route(
      "/api/simulation/results", QHttpServerRequest::Method::Get,
      [this, addCorsHeaders](const QHttpServerRequest &) {
        qDebug() << "📈 GET /api/simulation/results";
        return addCorsHeaders(m_apiHandler->handleGetSimulationResults());
      });

  // Export endpoints
  m_httpServer->route(
      "/api/export/results", QHttpServerRequest::Method::Post,
      [this, addCorsHeaders](const QHttpServerRequest &request) {
        qDebug() << "💾 POST /api/export/results";
        QJsonObject data = parseRequestBody(request);
        return addCorsHeaders(m_apiHandler->handleExportResults(data));
      });

  // Simple status endpoint
  m_httpServer->route("/status", QHttpServerRequest::Method::Get,
                      [addCorsHeaders](const QHttpServerRequest &) {
                        qDebug() << "🔍 GET /status";
                        QJsonObject response;
                        response["status"] =
                            "Train Simulation Backend C++ is running!";
                        response["service"] = "Qt Train Simulation HTTP API";
                        return addCorsHeaders(QHttpServerResponse(response));
                      });
}
