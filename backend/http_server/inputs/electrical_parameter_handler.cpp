#include "electrical_parameter_handler.h"

ElectricalParameterHandler::ElectricalParameterHandler(AppContext &context,
                                                       QObject *parent)
    : QObject{parent}, m_context(context) {}

QHttpServerResponse
ElectricalParameterHandler::handleGetElectricalParameters() {
  QJsonObject response;

  if (!m_context.powerData || !m_context.efficiencyData) {
    response["status"] = "error";
    response["message"] = "Electrical data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  QJsonObject electricalParams;
  electricalParams["wheelPower"] = m_context.powerData->p_wheel;
  electricalParams["motorOutPower"] = m_context.powerData->p_motorOut;
  electricalParams["motorInPower"] = m_context.powerData->p_motorIn;
  electricalParams["vvvfInPower"] = m_context.powerData->p_vvvfIn;
  electricalParams["catenaryPower"] = m_context.powerData->p_catenary;
  electricalParams["apsPower"] = m_context.powerData->p_aps;
  electricalParams["staticApsPower"] = m_context.powerData->stat_p_aps;

  // Add voltage parameters
  electricalParams["staticLineVoltage"] = m_context.energyData->stat_vol_line;
  electricalParams["staticMotorVoltage"] = m_context.energyData->stat_vol_motor;

  electricalParams["staticGearEfficiency"] =
      m_context.efficiencyData->stat_eff_gear;
  electricalParams["staticMotorEfficiency"] =
      m_context.efficiencyData->stat_eff_motor;
  electricalParams["staticVvvfEfficiency"] =
      m_context.efficiencyData->stat_eff_vvvf;

  response["electricalParameters"] = electricalParams;
  response["status"] = "success";

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse
ElectricalParameterHandler::handleUpdateElectricalParameters(
    const QJsonObject &data) {
  QJsonObject response;

  if (!m_context.powerData || !m_context.efficiencyData) {
    response["status"] = "error";
    response["message"] = "Electrical data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  try {
    qDebug() << "🔌 ELECTRICAL PARAMETERS UPDATE:";
    qDebug() << "Raw data:" << data;

    // Update electrical parameters directly from the data
    if (data.contains("stat_vol_line")) {
      m_context.energyData->stat_vol_line = data["stat_vol_line"].toDouble();
      qDebug() << "✅ Updated stat_vol_line:"
               << m_context.energyData->stat_vol_line;
    }
    if (data.contains("stat_vol_motor")) {
      m_context.energyData->stat_vol_motor = data["stat_vol_motor"].toDouble();
      qDebug() << "✅ Updated stat_vol_motor:"
               << m_context.energyData->stat_vol_motor;
    }
    if (data.contains("stat_eff_gear")) {
      double efficiency = data["stat_eff_gear"].toDouble();
      // Convert percentage to decimal if > 1 (assume percentage input)
      if (efficiency > 1.0) {
        efficiency = efficiency / 100.0;
      }
      m_context.efficiencyData->stat_eff_gear = efficiency;
      qDebug() << "✅ Updated stat_eff_gear:"
               << m_context.efficiencyData->stat_eff_gear;
    }
    if (data.contains("stat_eff_motor")) {
      double efficiency = data["stat_eff_motor"].toDouble();
      // Convert percentage to decimal if > 1 (assume percentage input)
      if (efficiency > 1.0) {
        efficiency = efficiency / 100.0;
      }
      m_context.efficiencyData->stat_eff_motor = efficiency;
      qDebug() << "✅ Updated stat_eff_motor:"
               << m_context.efficiencyData->stat_eff_motor;
    }
    if (data.contains("stat_eff_vvvf")) {
      double efficiency = data["stat_eff_vvvf"].toDouble();
      // Convert percentage to decimal if > 1 (assume percentage input)
      if (efficiency > 1.0) {
        efficiency = efficiency / 100.0;
      }
      m_context.efficiencyData->stat_eff_vvvf = efficiency;
      qDebug() << "✅ Updated stat_eff_vvvf:"
               << m_context.efficiencyData->stat_eff_vvvf;
    }
    if (data.contains("p_aps")) {
      m_context.powerData->p_aps = data["p_aps"].toDouble();
      qDebug() << "✅ Updated p_aps:" << m_context.powerData->p_aps;
    }
    if (data.contains("stat_pf")) {
      // Power factor - might need a specific field for this
      qDebug() << "✅ Received stat_pf:" << data["stat_pf"].toDouble();
    }

    response["status"] = "success";
    response["message"] = "Electrical parameters updated successfully";
    qDebug() << "🔌 Electrical parameters update completed successfully";
  } catch (const std::exception &e) {
    response["status"] = "error";
    response["message"] =
        QString("Error updating parameters: %1").arg(e.what());
  }

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

// #include "electrical_parameter_handler.h"

// ElectricalParameterHandler::ElectricalParameterHandler(AppContext &context,
//                                                        QObject *parent)
//     : QObject{parent}, m_context(context) {}

// QHttpServerResponse
// ElectricalParameterHandler::handleGetElectricalParameters() {
//   QJsonObject response;

//   if (!m_context.powerData || !m_context.efficiencyData) {
//     response["status"] = "error";
//     response["message"] = "Electrical data not initialized";
//     return QHttpServerResponse(
//         QJsonDocument(response).toJson(),
//         QHttpServerResponse::StatusCode::InternalServerError);
//   }

//   QJsonObject electricalParams;
//   electricalParams["wheelPower"] = m_context.powerData->p_wheel;
//   electricalParams["motorOutPower"] = m_context.powerData->p_motorOut;
//   electricalParams["motorInPower"] = m_context.powerData->p_motorIn;
//   electricalParams["vvvfInPower"] = m_context.powerData->p_vvvfIn;
//   electricalParams["catenaryPower"] = m_context.powerData->p_catenary;
//   electricalParams["apsPower"] = m_context.powerData->p_aps;
//   electricalParams["staticApsPower"] = m_context.powerData->stat_p_aps;

//   electricalParams["staticGearEfficiency"] =
//       m_context.efficiencyData->stat_eff_gear;
//   electricalParams["staticMotorEfficiency"] =
//       m_context.efficiencyData->stat_eff_motor;
//   electricalParams["staticVvvfEfficiency"] =
//       m_context.efficiencyData->stat_eff_vvvf;

//   response["electricalParameters"] = electricalParams;
//   response["status"] = "success";

//   return QHttpServerResponse(QJsonDocument(response).toJson(),
//                              QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse
// ElectricalParameterHandler::handleUpdateElectricalParameters(
//     const QJsonObject &data) {
//   QJsonObject response;

//   if (!m_context.powerData || !m_context.efficiencyData) {
//     response["status"] = "error";
//     response["message"] = "Electrical data not initialized";
//     return QHttpServerResponse(
//         QJsonDocument(response).toJson(),
//         QHttpServerResponse::StatusCode::InternalServerError);
//   }

//   try {
//     qDebug() << "🔌 ELECTRICAL PARAMETERS UPDATE:";
//     qDebug() << "Raw data:" << data;

//     // Check if energyData is initialized
//     if (!m_context.energyData) {
//       qDebug() << "❌ CRITICAL: energyData is NULL - cannot update voltage!";
//       response["status"] = "error";
//       response["message"] = "Energy data not initialized";
//       return QHttpServerResponse(
//           QJsonDocument(response).toJson(),
//           QHttpServerResponse::StatusCode::InternalServerError);
//     }

//     // Extract parameters from electricalParameters wrapper
//     QJsonObject params = data.contains("electricalParameters")
//                              ? data["electricalParameters"].toObject()
//                              : data;

//     // Update electrical parameters from the extracted params
//     if (params.contains("stat_vol_line")) {
//       m_context.energyData->stat_vol_line =
//       params["stat_vol_line"].toDouble(); qDebug() << "✅ Updated
//       stat_vol_line:"
//                << m_context.energyData->stat_vol_line
//                << "V (for static simulation)";

//       // Verify the value was actually set
//       if (m_context.energyData->stat_vol_line <= 0) {
//         qDebug() << "⚠️ WARNING: stat_vol_line is"
//                  << m_context.energyData->stat_vol_line
//                  << "- current calculation will fail!";
//       }
//     }
//     if (params.contains("stat_vol_motor")) {
//       m_context.energyData->stat_vol_motor =
//           params["stat_vol_motor"].toDouble();
//       qDebug() << "✅ Updated stat_vol_motor:"
//                << m_context.energyData->stat_vol_motor;
//     }
//     if (params.contains("stat_eff_gear")) {
//       m_context.efficiencyData->stat_eff_gear =
//           params["stat_eff_gear"].toDouble();
//       qDebug() << "✅ Updated stat_eff_gear:"
//                << m_context.efficiencyData->stat_eff_gear;
//     }
//     if (params.contains("stat_eff_motor")) {
//       m_context.efficiencyData->stat_eff_motor =
//           params["stat_eff_motor"].toDouble();
//       qDebug() << "✅ Updated stat_eff_motor:"
//                << m_context.efficiencyData->stat_eff_motor;
//     }
//     if (params.contains("stat_eff_vvvf")) {
//       m_context.efficiencyData->stat_eff_vvvf =
//           params["stat_eff_vvvf"].toDouble();
//       qDebug() << "✅ Updated stat_eff_vvvf:"
//                << m_context.efficiencyData->stat_eff_vvvf;
//     }
//     if (params.contains("p_aps")) {
//       m_context.powerData->p_aps = params["p_aps"].toDouble();
//       qDebug() << "✅ Updated p_aps:" << m_context.powerData->p_aps;
//     }
//     if (params.contains("stat_pf")) {
//       // Power factor - might need a specific field for this
//       qDebug() << "✅ Received stat_pf:" << params["stat_pf"].toDouble();
//     }

//     response["status"] = "success";
//     response["message"] = "Electrical parameters updated successfully";
//     qDebug() << "🔌 Electrical parameters update completed successfully";
//   } catch (const std::exception &e) {
//     response["status"] = "error";
//     response["message"] =
//         QString("Error updating parameters: %1").arg(e.what());
//   }

//   return QHttpServerResponse(QJsonDocument(response).toJson(),
//                              QHttpServerResponse::StatusCode::Ok);
// }
