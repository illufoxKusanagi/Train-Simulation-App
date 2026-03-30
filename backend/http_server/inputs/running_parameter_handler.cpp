#include "running_parameter_handler.h"
#include <QJsonDocument>

RunningParameterHandler::RunningParameterHandler(AppContext &context,
                                                 QObject *parent)
    : QObject{parent}, m_context(context) {}

QHttpServerResponse RunningParameterHandler::handleGetRunningParameters() {
  QJsonObject response;

  if (!m_context.movingData || !m_context.resistanceData) {
    response["status"] = "error";
    response["message"] = "Running data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  QJsonObject runningParams;

  runningParams["startRes"] = m_context.resistanceData->startRes;
  runningParams["v_diffCoast"] = m_context.movingData->v_diffCoast;
  runningParams["acc_start"] = m_context.movingData->acc_start;
  runningParams["v_p1"] = m_context.movingData->v_p1;
  runningParams["v_p2"] = m_context.movingData->v_p2;
  runningParams["v_b1"] = m_context.movingData->v_b1;
  runningParams["v_b2"] = m_context.movingData->v_b2;
  runningParams["decc_start"] = m_context.movingData->decc_start;
  runningParams["decc_emergency"] = m_context.movingData->decc_emergency;

  if (m_context.loadData) {
    runningParams["n_PTc"] = m_context.loadData->n_PTc;
    runningParams["n_PM1"] = m_context.loadData->n_PM1;
    runningParams["n_PM2"] = m_context.loadData->n_PM2;
    runningParams["n_PT1"] = m_context.loadData->n_PT1;
    runningParams["n_PT2"] = m_context.loadData->n_PT2;
    runningParams["n_PT3"] = m_context.loadData->n_PT3;
  }

  response["runningParameters"] = runningParams;
  response["status"] = "success";

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse RunningParameterHandler::handleUpdateRunningParameters(
    const QJsonObject &data) {
  QJsonObject response;

  if (!m_context.loadData || !m_context.movingData) {
    response["status"] = "error";
    response["message"] = "Running data not initialized";
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  try {
    QJsonObject runningParams = data.contains("runningParameters")
                                    ? data["runningParameters"].toObject()
                                    : data;

    if (runningParams.contains("startRes")) {
      m_context.resistanceData->startRes = runningParams["startRes"].toDouble();
    }
    if (runningParams.contains("v_diffCoast")) {
      m_context.movingData->v_diffCoast =
          runningParams["v_diffCoast"].toDouble();
    }
    if (runningParams.contains("acc_start")) {
      m_context.movingData->acc_start = runningParams["acc_start"].toDouble();
    }
    if (runningParams.contains("v_p1")) {
      m_context.movingData->v_p1 = runningParams["v_p1"].toDouble();
    }
    if (runningParams.contains("v_p2")) {
      m_context.movingData->v_p2 = runningParams["v_p2"].toDouble();
    }
    if (runningParams.contains("v_b1")) {
      m_context.movingData->v_b1 = runningParams["v_b1"].toDouble();
    }
    if (runningParams.contains("v_b2")) {
      m_context.movingData->v_b2 = runningParams["v_b2"].toDouble();
    }
    if (runningParams.contains("decc_start")) {
      m_context.movingData->decc_start = runningParams["decc_start"].toDouble();
    }
    if (runningParams.contains("decc_emergency")) {
      m_context.movingData->decc_emergency =
          runningParams["decc_emergency"].toDouble();
    }

    if (m_context.loadData) {
      if (runningParams.contains("n_PTc")) {
        m_context.loadData->n_PTc = runningParams["n_PTc"].toDouble();
      }
      if (runningParams.contains("n_PM1")) {
        m_context.loadData->n_PM1 = runningParams["n_PM1"].toDouble();
      }
      if (runningParams.contains("n_PM2")) {
        m_context.loadData->n_PM2 = runningParams["n_PM2"].toDouble();
      }
      if (runningParams.contains("n_PT1") || runningParams.contains("n_Pt1")) {
        m_context.loadData->n_PT1 = runningParams.contains("n_PT1")
                                        ? runningParams["n_PT1"].toDouble()
                                        : runningParams["n_Pt1"].toDouble();
      }
      if (runningParams.contains("n_PT2") || runningParams.contains("n_Pt2")) {
        m_context.loadData->n_PT2 = runningParams.contains("n_PT2")
                                        ? runningParams["n_PT2"].toDouble()
                                        : runningParams["n_Pt2"].toDouble();
      }
      if (runningParams.contains("n_PT3") || runningParams.contains("n_Pt3")) {
        m_context.loadData->n_PT3 = runningParams.contains("n_PT3")
                                        ? runningParams["n_PT3"].toDouble()
                                        : runningParams["n_Pt3"].toDouble();
      }
    }

    response["status"] = "success";
    response["message"] = "Running parameters updated successfully";
  } catch (const std::exception &e) {
    response["status"] = "error";
    response["message"] =
        QString("Error updating parameters: %1").arg(e.what());
  }

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}
