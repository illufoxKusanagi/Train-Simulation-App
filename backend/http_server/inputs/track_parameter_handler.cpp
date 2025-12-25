#include "track_parameter_handler.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <iostream>

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
        double dist = val.toDouble();
        // Filter out 0-distance station (starting point) to prevent immediate
        // braking
        if (dist > 1e-6) {
          m_context.stationData->x_station.push_back(dist);
        }
      }
      // DO NOT SORT x_station. It represents ordered segment lengths, not
      // positions. std::sort(m_context.stationData->x_station.begin(),
      // m_context.stationData->x_station.end());

      // Sync n_station with the vector size (Targets + Start)
      if (!m_context.stationData->x_station.empty()) {
        m_context.stationData->n_station =
            m_context.stationData->x_station.size() + 1;
      }

      std::cout << "DATA_DEBUG: x_station (Segment Distances) size: "
                << m_context.stationData->x_station.size() << std::endl;
      if (!m_context.stationData->x_station.empty()) {
        QString stationsStr;
        for (double val : m_context.stationData->x_station)
          stationsStr += QString::number(val) + ", ";
        std::cout << "DATA_DEBUG: Segments: " << stationsStr.toStdString()
                  << std::endl;
      }
    }

    // Cumulative Station Distance (Column 1)
    if (trackParams.contains("tot_x_station_array") &&
        trackParams["tot_x_station_array"].isArray()) {
      QJsonArray arr = trackParams["tot_x_station_array"].toArray();
      m_context.stationData->tot_x_station.clear();
      for (const auto &val : arr) {
        m_context.stationData->tot_x_station.push_back(val.toDouble());
      }

      // Sync n_station if x_station was not provided but tot_x_station was
      // (fallback)
      if (m_context.stationData->x_station.empty() &&
          !m_context.stationData->tot_x_station.empty()) {
        m_context.stationData->n_station =
            m_context.stationData->tot_x_station.size();
      }
    }

    // Detailed Track Parameters (Start, End, Value)
    // Speed Limit
    if (trackParams.contains("x_v_limitStart_array") &&
        trackParams["x_v_limitStart_array"].isArray()) {
      QJsonArray arr = trackParams["x_v_limitStart_array"].toArray();
      m_context.stationData->x_v_limitStart.clear();
      for (const auto &val : arr) {
        m_context.stationData->x_v_limitStart.push_back(val.toDouble());
      }
    }
    if (trackParams.contains("x_v_limitEnd_array") &&
        trackParams["x_v_limitEnd_array"].isArray()) {
      QJsonArray arr = trackParams["x_v_limitEnd_array"].toArray();
      m_context.stationData->x_v_limitEnd.clear();
      for (const auto &val : arr) {
        m_context.stationData->x_v_limitEnd.push_back(val.toDouble());
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

    // Slope
    if (trackParams.contains("x_slopeStart_array") &&
        trackParams["x_slopeStart_array"].isArray()) {
      QJsonArray arr = trackParams["x_slopeStart_array"].toArray();
      m_context.stationData->x_slopeStart.clear();
      for (const auto &val : arr) {
        m_context.stationData->x_slopeStart.push_back(val.toDouble());
      }
    }
    if (trackParams.contains("x_slopeEnd_array") &&
        trackParams["x_slopeEnd_array"].isArray()) {
      QJsonArray arr = trackParams["x_slopeEnd_array"].toArray();
      m_context.stationData->x_slopeEnd.clear();
      for (const auto &val : arr) {
        m_context.stationData->x_slopeEnd.push_back(val.toDouble());
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

    // Radius
    if (trackParams.contains("x_radiusStart_array") &&
        trackParams["x_radiusStart_array"].isArray()) {
      QJsonArray arr = trackParams["x_radiusStart_array"].toArray();
      m_context.stationData->x_radiusStart.clear();
      for (const auto &val : arr) {
        m_context.stationData->x_radiusStart.push_back(val.toDouble());
      }
    }
    if (trackParams.contains("x_radiusEnd_array") &&
        trackParams["x_radiusEnd_array"].isArray()) {
      QJsonArray arr = trackParams["x_radiusEnd_array"].toArray();
      m_context.stationData->x_radiusEnd.clear();
      for (const auto &val : arr) {
        m_context.stationData->x_radiusEnd.push_back(val.toDouble());
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

    // ADDED: Include debug info in the response message
    QString debugMsg = "Track parameters updated successfully. ";
    if (!m_context.stationData->x_station.empty()) {
      debugMsg += QString("Stations: %1, First: %2")
                      .arg(m_context.stationData->x_station.size())
                      .arg(m_context.stationData->x_station[0]);
    } else {
      debugMsg += "Warning: No stations in x_station vector.";
    }

    response["status"] = "success";
    response["message"] = debugMsg;
    // qDebug() << "✅ Track parameters updated successfully";
  } catch (const std::exception &e) {
    // qDebug() << "💥 Exception in handleUpdateTrackParameters:" << e.what();
    std::cout << "Exception in handleUpdateTrackParameters: " << e.what()
              << std::endl;
    response["status"] = "error";
    response["message"] =
        QString("Error updating parameters: %1").arg(e.what());
  }

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}