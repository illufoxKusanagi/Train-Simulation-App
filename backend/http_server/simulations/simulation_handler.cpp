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

    // Use the helper methods based on the current simulation type
    if (m_currentSimulationType == "static") {
      response["results"] = getStaticResults().value("data");
      response["trackDistanceTable"] =
          getStaticResults().value("trackDistanceTable");
    } else {
      // Default to dynamic if not specified or explicitly dynamic
      response["results"] = getDynamicResults().value("data");
    }

    // Add debug info if needed (simplified for now to fix build)
    QJsonObject debugInfo;
    debugInfo["simulationType"] = m_currentSimulationType;
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

  // Build results array (limit to prevent huge payloads if needed, but
  // keeping full for now as per request)
  QJsonArray resultsArray;
  int dataSize = simulationDatas->time.size();

  for (int i = 0; i < dataSize; i++) {
    QJsonObject point;
    point["phase"] = simulationDatas->phase[i];
    point["time"] = simulationDatas->time[i];
    point["timeTotal"] = simulationDatas->timeTotal[i];
    point["distances"] = simulationDatas->distance[i];
    point["distancesTotal"] = simulationDatas->distanceTotal[i];
    // These arrays may have different sizes (populated once per simulation vs
    // once per iteration)
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
    point["powerMotorIn"] = simulationDatas->powerMotorIn[i];
    point["vvvfPowers"] = simulationDatas->vvvfPowers[i];

    // Add debug information for current values at each point
    // if (i < simulationDatas->vvvfCurrents.size()) {
    point["vvvfCurrents"] = simulationDatas->vvvfCurrents[i];
    //   point["vvvfCurrentsDebug"] = QString("Value at index %1: %2")
    //                                    .arg(i)
    //                                    .arg(simulationDatas->vvvfCurrents[i]);
    // } else {
    //   point["vvvfCurrents"] = QJsonValue();
    //   point["vvvfCurrentsDebug"] =
    //       QString("Index %1 >= array size %2")
    //           .arg(i)
    //           .arg(simulationDatas->vvvfCurrents.size());
    // }

    // if (i < simulationDatas->catenaryCurrents.size()) {
    point["catenaryCurrents"] = simulationDatas->catenaryCurrents[i];
    //   point["catenaryCurrentsDebug"] =
    //       QString("Value at index %1: %2")
    //           .arg(i)
    //           .arg(simulationDatas->catenaryCurrents[i]);
    // } else {
    //   point["catenaryCurrents"] = QJsonValue();
    //   point["catenaryCurrentsDebug"] =
    //       QString("Index %1 >= array size %2")
    //           .arg(i)
    //           .arg(simulationDatas->catenaryCurrents.size());
    // }
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

  // Static simulation also needs time-series data for charts
  QJsonArray resultsArray;
  int dataSize = simulationDatas->time.size();

  for (int i = 0; i < dataSize; i++) {
    QJsonObject point;
    point["phase"] = simulationDatas->phase[i];
    point["time"] = simulationDatas->time[i];
    point["timeTotal"] = simulationDatas->timeTotal[i];
    point["distances"] = simulationDatas->distance[i];
    point["distancesTotal"] = simulationDatas->distanceTotal[i];
    // These arrays may have different sizes (populated once per simulation vs
    // once per iteration)
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
    point["powerMotorIn"] = simulationDatas->powerMotorIn[i];
    point["vvvfPowers"] = simulationDatas->vvvfPowers[i];

    // Add debug information for current values at each point
    // if (i < simulationDatas->vvvfCurrents.size()) {
    point["vvvfCurrents"] = simulationDatas->vvvfCurrents[i];
    //   point["vvvfCurrentsDebug"] = QString("Value at index %1: %2")
    //                                    .arg(i)
    //                                    .arg(simulationDatas->vvvfCurrents[i]);
    // } else {
    // point["vvvfC?urrents"] = QJsonValue();
    // point["vvvfCurrentsDebug"] =
    //       QString("Index %1 >= array size %2")
    //           .arg(i)
    //           .arg(simulationDatas->vvvfCurrents.size());
    // }

    // if (i < simulationDatas->catenaryCurrents.size()) {
    point["catenaryCurrents"] = simulationDatas->catenaryCurrents[i];
    //   point["catenaryCurrentsDebug"] =
    //       QString("Value at index %1: %2")
    //           .arg(i)
    //           .arg(simulationDatas->catenaryCurrents[i]);
    // // } else {
    //   point["catenaryCurrents"] = QJsonValue();
    //   point["catenaryCurrentsDebug"] =
    //       QString("Index %1 >= array size %2")
    //           .arg(i)
    //           .arg(simulationDatas->catenaryCurrents.size());
    // }

    point["catenaryPowers"] = simulationDatas->catenaryPowers[i];
    point["energyConsumptions"] = simulationDatas->energyConsumptions[i];
    point["energyPowerings"] = simulationDatas->energyPowerings[i];
    point["energyRegenerations"] = simulationDatas->energyRegenerations[i];
    point["energyAps"] = simulationDatas->energyAps[i];
    point["energyCatenaries"] = simulationDatas->energyCatenaries[i];
    point["motorResistancesZero"] = simulationDatas->motorResistancesZero[i];
    point["motorResistancesFive"] = simulationDatas->motorResistancesFive[i];
    point["motorResistancesTen"] = simulationDatas->motorResistancesTen[i];
    point["motorResistancesTwentyFive"] =
        simulationDatas->motorResistancesTwentyFive[i];

    resultsArray.append(point);
  }

  // for (int i = 0; i < dataSize; i++) {
  //   QJsonObject point;
  //   point["time"] = simulationDatas->time[i];
  //   point["speed"] = simulationDatas->trainSpeeds[i];
  //   point["distance"] = simulationDatas->distance[i];
  //   point["acceleration"] = simulationDatas->accelerations[i];
  //   point["tractiveEffort"] = simulationDatas->tractionEfforts[i];

  //   resultsArray.append(point);
  // }
  results["data"] = resultsArray;

  // Calculate Track Distance Table (Logic from OutputTableHandler)
  QJsonObject trackDistanceTable;
  QJsonArray normalBraking;
  QJsonArray emergencyBraking;

  double vLimit = m_context.stationData->stat_v_limit;

  normalBraking.append(m_trainSimulation->m_simulationTrackHandler
                           ->calculateNormalSimulationTrack(vLimit));
  normalBraking.append(m_trainSimulation->m_simulationTrackHandler
                           ->calculateDelaySimulationTrack(vLimit));
  normalBraking.append(m_trainSimulation->m_simulationTrackHandler
                           ->calculateSafetySimulationTrack(vLimit));

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
      // Check if we have results (either maxSpeed > 0 or errors present)
      if (maxSpeed > 0 || !m_trainSimulation->getSimulationErrors().isEmpty()) {
        response["simulationStatus"] = "completed";
        response["hasResults"] = true;

        // --- Populate Summary Data ---
        QJsonObject summary;
        summary["maxSpeed"] = maxSpeed;
        summary["distanceTravelled"] =
            m_trainSimulation->getDistanceTravelled();
        summary["maxTractionEffort"] =
            m_trainSimulation->getMaxTractionEffort();
        summary["maxEnergyConsumption"] =
            m_trainSimulation->getMaxEnergyConsumption();

        // if (m_currentSimulationType == "dynamic") {
        summary["maxCatenaryPower"] = m_trainSimulation->getMaxCatenaryPower();
        summary["maxVvvfPower"] = m_trainSimulation->getMaxVvvfPower();
        summary["maxCatenaryCurrent"] =
            m_trainSimulation->getMaxCatenaryCurrent();
        summary["maxVvvfCurrent"] = m_trainSimulation->getMaxVvvfCurrent();
        // summary["maxEnergyRegen"] = m_trainSimulation->getMaxEnergyRegen();
        summary["adhesion"] = m_trainSimulation->getAdhesion();
        // }

        response["summary"] = summary;

        // --- Populate Warnings & Errors ---
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

        // --- Populate Detailed Results based on Type ---
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
