#include "simulation_handler.h"
#include <exception>
#include <qcontainerfwd.h>
#include <qjsonarray.h>
#include <qjsonobject.h>

SimulationHandler::SimulationHandler(AppContext &context, QObject *parent)
    : QObject(parent), m_context(context) {
  m_trainSimulation = new TrainSimulationHandler(context, this);
}

QHttpServerResponse
SimulationHandler::handleStartSimulation(const QJsonObject &data) {
  QJsonObject response;
  try {
    m_trainSimulation->clearWarnings();
    m_currentSimulationType = data.value("type").toString("dynamic");

    if (m_currentSimulationType == "static") {
      m_trainSimulation->simulateStaticTrainMovement();
    } else {
      m_trainSimulation->simulateDynamicTrainMovement();
    }
    QStringList errors = m_trainSimulation->getSimulationErrors();
    if (!errors.isEmpty()) {
      response["status"] = "error";
      response["message"] = "Simulation failed to start.";
      QJsonArray errorArray;
      for (const QString &error : errors) {
        errorArray.append(error);
      }
      response["errors"] = errorArray;
      return QHttpServerResponse(QJsonDocument(response).toJson(),
                                 QHttpServerResponse::StatusCode::Ok);
    }
    response["status"] = "success";
    response["message"] = "Simulation started";
    response["simulationType"] = m_currentSimulationType;
    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);

  } catch (const std::exception &e) {
    response["status"] = "error";
    response["message"] = QString("Simulation error: %1").arg(e.what());
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }
}

QHttpServerResponse SimulationHandler::handleGetSimulationResults() {
  QJsonObject response;
  try {
    response["status"] = "success";

    if (m_currentSimulationType == "static") {
      QJsonObject staticResults = getStaticResults();
      response["results"] = staticResults.value("data");
      response["trackDistanceTable"] =
          staticResults.value("trackDistanceTable");
    } else {
      response["results"] = getDynamicResults().value("data");
    }

    QJsonObject debugInfo;
    debugInfo["simulationType"] = m_currentSimulationType;

    QJsonArray debugLogs;
    for (const QString &log : m_trainSimulation->getDebugLogs()) {
      debugLogs.append(log);
    }
    debugInfo["logs"] = debugLogs;
    response["debugInfo"] = debugInfo;

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
  } catch (const std::exception &e) {
    response["status"] = "error";
    response["message"] = QString("Error retrieving results: %1").arg(e.what());
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }
}
QJsonObject SimulationHandler::getDynamicResults() {
  QJsonObject results;
  auto simulationDatas = m_context.simulationDatas.data();

  QJsonArray resultsArray;
  int dataSize = simulationDatas->time.size();

  for (int i = 0; i < dataSize; i++) {
    QJsonObject point;
    point["phase"] = simulationDatas->phase[i];
    point["time"] = simulationDatas->time[i];
    point["timeTotal"] = simulationDatas->timeTotal[i];
    point["distances"] = simulationDatas->distance[i];
    point["distancesTotal"] = simulationDatas->distanceTotal[i];
    point["odos"] =
        i < simulationDatas->odos.size() ? simulationDatas->odos[i] : 0.0;
    point["brakingDistances"] = i < simulationDatas->brakingDistances.size()
                                    ? simulationDatas->brakingDistances[i]
                                    : 0.0;
    point["slopes"] =
        i < simulationDatas->slopes.size() ? simulationDatas->slopes[i] : 0.0;
    point["radiuses"] = i < simulationDatas->radiuses.size()
                            ? simulationDatas->radiuses[i]
                            : 0.0;
    point["speeds"] = simulationDatas->trainSpeeds[i];
    point["speedLimits"] = i < simulationDatas->speedLimits.size()
                               ? simulationDatas->speedLimits[i]
                               : 0.0;
    point["speedsSi"] = simulationDatas->trainSpeedsSi[i];
    point["accelerations"] = simulationDatas->accelerations[i];
    point["accelerationsSi"] = simulationDatas->accelerationsSi[i];
    point["motorForce"] = simulationDatas->motorForce[i];
    point["motorResistance"] = simulationDatas->motorResistance[i];
    point["totalResistance"] = simulationDatas->totalResistance[i];
    point["tractionForcePerMotor"] = simulationDatas->tractionForcePerMotor[i];
    point["resistancePerMotor"] = simulationDatas->resistancePerMotor[i];
    point["torque"] = simulationDatas->torque[i];
    point["rpm"] = simulationDatas->rpm[i];
    point["powerWheel"] = simulationDatas->powerWheel[i];
    point["powerMotorOut"] = simulationDatas->powerMotorOut[i];
    point["powerMotorOutputPerMotor"] =
        simulationDatas->powerMotorOutPerMotor[i];
    point["powerMotorIn"] = simulationDatas->powerMotorIn[i];
    point["vvvfPowers"] = simulationDatas->vvvfPowers[i];

    point["vvvfCurrents"] = simulationDatas->vvvfCurrents[i];
    point["catenaryCurrents"] = simulationDatas->catenaryCurrents[i];
    point["catenaryPowers"] = simulationDatas->catenaryPowers[i];
    point["energyConsumptions"] = simulationDatas->energyConsumptions[i];
    point["energyPowerings"] = simulationDatas->energyPowerings[i];
    point["energyRegenerations"] = simulationDatas->energyRegenerations[i];
    point["energyAps"] = simulationDatas->energyAps[i];
    point["energyCatenaries"] = simulationDatas->energyCatenaries[i];

    resultsArray.append(point);
  }
  results["data"] = resultsArray;
  return results;
}

QJsonObject SimulationHandler::getStaticResults() {
  QJsonObject results;
  auto simulationDatas = m_context.simulationDatas.data();

  QJsonArray resultsArray;
  int dataSize = simulationDatas->time.size();

  for (int i = 0; i < dataSize; i++) {
    QJsonObject point;
    point["phase"] = simulationDatas->phase[i];
    point["time"] = simulationDatas->time[i];
    point["timeTotal"] = simulationDatas->timeTotal[i];
    point["distances"] = simulationDatas->distance[i];
    point["distancesTotal"] = simulationDatas->distanceTotal[i];
    point["odos"] =
        i < simulationDatas->odos.size() ? simulationDatas->odos[i] : 0.0;
    point["brakingDistances"] = i < simulationDatas->brakingDistances.size()
                                    ? simulationDatas->brakingDistances[i]
                                    : 0.0;
    point["slopes"] =
        i < simulationDatas->slopes.size() ? simulationDatas->slopes[i] : 0.0;
    point["radiuses"] = i < simulationDatas->radiuses.size()
                            ? simulationDatas->radiuses[i]
                            : 0.0;
    point["speeds"] = simulationDatas->trainSpeeds[i];
    point["speedLimits"] = i < simulationDatas->speedLimits.size()
                               ? simulationDatas->speedLimits[i]
                               : 0.0;
    point["speedsSi"] = simulationDatas->trainSpeedsSi[i];
    point["accelerations"] = simulationDatas->accelerations[i];
    point["accelerationsSi"] = simulationDatas->accelerationsSi[i];
    point["motorForce"] = simulationDatas->motorForce[i];
    point["motorResistance"] = simulationDatas->motorResistance[i];
    point["totalResistance"] = simulationDatas->totalResistance[i];
    point["tractionForcePerMotor"] = simulationDatas->tractionForcePerMotor[i];
    point["resistancePerMotor"] = simulationDatas->resistancePerMotor[i];
    point["torque"] = simulationDatas->torque[i];
    point["rpm"] = simulationDatas->rpm[i];
    point["powerWheel"] = simulationDatas->powerWheel[i];
    point["powerMotorOut"] = simulationDatas->powerMotorOut[i];
    point["powerMotorOutputPerMotor"] =
        simulationDatas->powerMotorOutPerMotor[i];
    point["powerMotorIn"] = simulationDatas->powerMotorIn[i];
    point["vvvfPowers"] = simulationDatas->vvvfPowers[i];

    point["vvvfCurrents"] = simulationDatas->vvvfCurrents[i];
    point["catenaryCurrents"] = simulationDatas->catenaryCurrents[i];
    point["catenaryPowers"] = simulationDatas->catenaryPowers[i];
    point["energyConsumptions"] = simulationDatas->energyConsumptions[i];
    point["energyPowerings"] = simulationDatas->energyPowerings[i];
    point["energyRegenerations"] = simulationDatas->energyRegenerations[i];
    point["energyAps"] = simulationDatas->energyAps[i];
    point["energyCatenaries"] = simulationDatas->energyCatenaries[i];

    if (m_currentSimulationType == "static") {
      point["motorResistancesOption1"] =
          i < simulationDatas->motorResistancesOption1.size()
              ? simulationDatas->motorResistancesOption1[i]
              : 0.0;
      point["motorResistancesOption2"] =
          i < simulationDatas->motorResistancesOption2.size()
              ? simulationDatas->motorResistancesOption2[i]
              : 0.0;
      point["motorResistancesOption3"] =
          i < simulationDatas->motorResistancesOption3.size()
              ? simulationDatas->motorResistancesOption3[i]
              : 0.0;
      point["motorResistancesOption4"] =
          i < simulationDatas->motorResistancesOption4.size()
              ? simulationDatas->motorResistancesOption4[i]
              : 0.0;
    }

    resultsArray.append(point);
  }

  results["data"] = resultsArray;

  QJsonObject trackDistanceTable;
  QJsonArray normalBraking;
  QJsonArray emergencyBraking;

  normalBraking.append(m_trainSimulation->m_simulationTrackHandler
                           ->calculateNormalSimulationTrack());
  normalBraking.append(m_trainSimulation->m_simulationTrackHandler
                           ->calculateDelaySimulationTrack());
  normalBraking.append(m_trainSimulation->m_simulationTrackHandler
                           ->calculateSafetySimulationTrack());

  emergencyBraking.append(m_trainSimulation->m_simulationTrackHandler
                              ->calculateEmergencyNormalSimulationTrack());
  emergencyBraking.append(m_trainSimulation->m_simulationTrackHandler
                              ->calculateEmergencyDelaySimulationTrack());
  emergencyBraking.append(m_trainSimulation->m_simulationTrackHandler
                              ->calculateEmergencySafetySimulationTrack());

  trackDistanceTable["normalBraking"] = normalBraking;
  trackDistanceTable["emergencyBraking"] = emergencyBraking;
  trackDistanceTable["headers"] =
      QJsonArray({"Normal Braking", "Emergency Braking"});
  trackDistanceTable["labels"] =
      QJsonArray({"Normal", "Delay 3s", "Safety 20%"});

  results["trackDistanceTable"] = trackDistanceTable;
  return results;
}

QHttpServerResponse SimulationHandler::handleGetSimulationStatus() {
  QJsonObject response;
  response["status"] = "success";

  if (!m_trainSimulation) {
    response["isRunning"] = false;
    response["simulationStatus"] = "unavailable";
    response["message"] = "Simulation handler not initialized";
  } else {
    bool isRunning = m_trainSimulation->isSimulationRunning();
    response["isRunning"] = isRunning;

    if (isRunning) {
      response["simulationStatus"] = "running";
      response["hasResults"] = false;
    } else {
      double maxSpeed = m_trainSimulation->getMaxSpeed();
      if (maxSpeed > 0 || !m_trainSimulation->getSimulationErrors().isEmpty()) {
        response["simulationStatus"] = "completed";
        response["hasResults"] = true;

        QJsonObject summary;
        summary["maxSpeed"] = maxSpeed;
        summary["distanceTravelled"] =
            m_trainSimulation->getDistanceTravelled();
        summary["maxTractionEffort"] =
            m_trainSimulation->getMaxTractionEffort();
        summary["maxEnergyConsumption"] =
            m_trainSimulation->getTotalEnergyConsumption();

        summary["maxCatenaryPower"] = m_trainSimulation->getMaxCatenaryPower();
        summary["maxVvvfPower"] = m_trainSimulation->getMaxVvvfPower();
        summary["maxMotorPowerPerMotor"] =
            m_trainSimulation->getMaxMotorPowerPerMotor();
        summary["maxCatenaryCurrent"] =
            m_trainSimulation->getMaxCatenaryCurrent();
        summary["maxVvvfCurrent"] = m_trainSimulation->getMaxVvvfCurrent();
        summary["maxPowerTime"] = m_trainSimulation->getMaxPowTime();
        summary["adhesion"] = m_trainSimulation->getAdhesion();

        if (m_currentSimulationType == "static") {
          double vLimit = m_context.stationData->stat_v_limit;
          summary["distanceOnBraking"] =
              m_trainSimulation->m_simulationTrackHandler
                  ->calculateStaticBrakingTrack();
          summary["distanceOnEmergencyBraking"] =
              m_trainSimulation->m_simulationTrackHandler
                  ->calculateBrakingEmergencyTrack();
        }

        response["summary"] = summary;

        QJsonArray warnings;
        for (const QString &warning :
             m_trainSimulation->getSimulationWarnings()) {
          warnings.append(warning);
        }
        response["warnings"] = warnings;

        QJsonArray errors;
        for (const QString &error : m_trainSimulation->getSimulationErrors()) {
          errors.append(error);
        }
        response["errors"] = errors;

        if (m_currentSimulationType == "static") {
          response["results"] = getStaticResults();
        } else {
          response["results"] = getDynamicResults();
        }

      } else {
        response["simulationStatus"] = "idle";
        response["hasResults"] = false;
      }
    }
  }

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}
