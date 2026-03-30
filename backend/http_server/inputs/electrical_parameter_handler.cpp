#include "electrical_parameter_handler.h"
#include "core/appcontext.h"
#include <QJsonDocument>

ElectricalParameterHandler::ElectricalParameterHandler(AppContext &context,
                                                       QObject *parent)
    : QObject{parent}, m_context(context) {}

QHttpServerResponse
ElectricalParameterHandler::handleGetElectricalParameters() {
  QJsonObject response;

  if (!m_context.powerData || !m_context.efficiencyData ||
      !m_context.energyData) {
    response["status"] = "error";
    response["message"] = "Electrical data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  QJsonObject electricalParams;
  electricalParams["stat_vol_line"] = m_context.energyData->stat_vol_line;
  electricalParams["stat_vol_motor"] = m_context.energyData->stat_vol_motor;
  electricalParams["p_aps"] = m_context.powerData->p_aps;
  electricalParams["stat_eff_gear"] =
      m_context.efficiencyData->stat_eff_gear * 100.0;
  electricalParams["stat_eff_motor"] =
      m_context.efficiencyData->stat_eff_motor * 100.0;
  electricalParams["stat_eff_vvvf"] =
      m_context.efficiencyData->stat_eff_vvvf * 100.0;

  response["electricalParameters"] = electricalParams;
  response["status"] = "success";

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse
ElectricalParameterHandler::handleUpdateElectricalParameters(
    const QJsonObject &data) {
  QJsonObject response;

  if (!m_context.powerData || !m_context.efficiencyData ||
      !m_context.energyData) {
    response["status"] = "error";
    response["message"] = "Electrical data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  try {
    if (!m_context.energyData) {
      response["status"] = "error";
      response["message"] = "Energy data not initialized";
      return QHttpServerResponse(
          QJsonDocument(response).toJson(),
          QHttpServerResponse::StatusCode::InternalServerError);
    }

    QJsonObject params = data.contains("electricalParameters")
                             ? data["electricalParameters"].toObject()
                             : data;

    if (params.contains("stat_vol_line")) {
      m_context.energyData->stat_vol_line = params["stat_vol_line"].toDouble();
      if (m_context.energyData->stat_vol_line <= 0) {
        m_context.simulationWarnings.append("WARN_STAT_VOL_LINE_ZERO");
      }
    }
    if (params.contains("stat_vol_motor")) {
      m_context.energyData->stat_vol_motor =
          params["stat_vol_motor"].toDouble();
      if (m_context.energyData->stat_vol_motor <= 0) {
        m_context.simulationWarnings.append("WARN_STAT_VOL_MOTOR_ZERO");
      }
    }
    if (params.contains("stat_eff_gear")) {
      m_context.efficiencyData->stat_eff_gear =
          params["stat_eff_gear"].toDouble() / 100.0;
      if (m_context.efficiencyData->stat_eff_gear <= 0) {
        m_context.simulationWarnings.append("WARN_STAT_EFF_GEAR_ZERO");
      }
    }
    if (params.contains("stat_eff_motor")) {
      m_context.efficiencyData->stat_eff_motor =
          params["stat_eff_motor"].toDouble() / 100.0;
      if (m_context.efficiencyData->stat_eff_motor <= 0) {
        m_context.simulationWarnings.append("WARN_STAT_EFF_MOTOR_ZERO");
      }
    }
    if (params.contains("stat_eff_vvvf")) {
      m_context.efficiencyData->stat_eff_vvvf =
          params["stat_eff_vvvf"].toDouble() / 100.0;
      if (m_context.efficiencyData->stat_eff_vvvf <= 0) {
        m_context.simulationWarnings.append("WARN_STAT_EFF_VVVF_ZERO");
      }
    }
    if (params.contains("p_aps")) {
      m_context.powerData->p_aps = params["p_aps"].toDouble();
      if (m_context.powerData->p_aps <= 0) {
        m_context.simulationWarnings.append("WARN_APS_POWER_ZERO");
      }
    }
    response["status"] = "success";
    response["message"] = "Electrical parameters updated successfully";
    response["datas"] = params;
  } catch (const std::exception &e) {
    response["status"] = "error";
    response["message"] =
        QString("Error updating parameters: %1").arg(e.what());
  }

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}
