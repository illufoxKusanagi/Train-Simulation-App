#include "track_parameter_handler.h"
#include <QJsonArray>
#include <QJsonDocument>

TrackParameterHandler::TrackParameterHandler(AppContext &context,
                                             QObject *parent)
    : QObject{parent}, m_context(context) {}

QHttpServerResponse TrackParameterHandler::handleGetTrackParameters() {
  QJsonObject response;

  if (!m_context.stationData) {
    response["status"] = "error";
    response["message"] = "Track data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  QJsonObject trackParams;

  // Use YOUR exact field names from TrackParams interface
  trackParams["n_station"] = m_context.stationData->n_station;
  trackParams["x_station"] = m_context.stationData->stat_x_station;
  trackParams["radius"] = m_context.stationData->stat_radius;
  trackParams["slope"] = m_context.stationData->stat_slope;
  trackParams["v_limit"] = m_context.stationData->stat_v_limit;
  trackParams["dwellTime"] = m_context.stationData->stat_dwellTime;

  response["trackParameters"] = trackParams;
  response["status"] = "success";

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse
TrackParameterHandler::handleUpdateTrackParameters(const QJsonObject &data) {
  QJsonObject response;

  if (!m_context.stationData) {
    response["status"] = "error";
    response["message"] = "Track data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  try {
    QJsonObject trackParams = data.contains("trackParameters")
                                  ? data["trackParameters"].toObject()
                                  : data;
    qDebug() << "📝 Updating track parameters:" << trackParams;

    // Use YOUR exact field names from TrackParams interface
    if (trackParams.contains("n_station")) {
      m_context.stationData->n_station = trackParams["n_station"].toInt();
    }
    if (trackParams.contains("x_station")) {
      m_context.stationData->stat_x_station =
          trackParams["x_station"].toDouble();
    }
    if (trackParams.contains("radius")) {
      m_context.stationData->stat_radius = trackParams["radius"].toDouble();
    }
    if (trackParams.contains("slope")) {
      m_context.stationData->stat_slope = trackParams["slope"].toDouble();
    }
    if (trackParams.contains("v_limit")) {
      m_context.stationData->stat_v_limit = trackParams["v_limit"].toDouble();
      m_context.movingData->v_limit = trackParams["v_limit"].toDouble();
    }
    if (trackParams.contains("dwellTime")) {
      m_context.stationData->stat_dwellTime =
          trackParams["dwellTime"].toDouble();
    }

    // Handle CSV array data for vectors
    if (trackParams.contains("x_station_array") &&
        trackParams["x_station_array"].isArray()) {
      QJsonArray arr = trackParams["x_station_array"].toArray();
      m_context.stationData->x_station.clear();
      for (const auto &val : arr) {
        m_context.stationData->x_station.push_back(val.toDouble());
      }
    }
    if (trackParams.contains("v_limit_array") &&
        trackParams["v_limit_array"].isArray()) {
      QJsonArray arr = trackParams["v_limit_array"].toArray();
      m_context.stationData->v_limit.clear();
      for (const auto &val : arr) {
        m_context.stationData->v_limit.push_back(val.toDouble());
      }
    }
    if (trackParams.contains("slope_array") &&
        trackParams["slope_array"].isArray()) {
      QJsonArray arr = trackParams["slope_array"].toArray();
      m_context.stationData->slope.clear();
      for (const auto &val : arr) {
        m_context.stationData->slope.push_back(val.toDouble());
      }
    }
    if (trackParams.contains("radius_array") &&
        trackParams["radius_array"].isArray()) {
      QJsonArray arr = trackParams["radius_array"].toArray();
      m_context.stationData->radius.clear();
      for (const auto &val : arr) {
        m_context.stationData->radius.push_back(val.toDouble());
      }
    }
    if (trackParams.contains("dwellTime_array") &&
        trackParams["dwellTime_array"].isArray()) {
      QJsonArray arr = trackParams["dwellTime_array"].toArray();
      m_context.stationData->dwellTime.clear();
      for (const auto &val : arr) {
        m_context.stationData->dwellTime.push_back(val.toDouble());
      }
    }

    response["status"] = "success";
    response["message"] = "Track parameters updated successfully";
    qDebug() << "✅ Track parameters updated successfully";
  } catch (const std::exception &e) {
    qDebug() << "💥 Exception in handleUpdateTrackParameters:" << e.what();
    response["status"] = "error";
    response["message"] =
        QString("Error updating parameters: %1").arg(e.what());
  }

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}