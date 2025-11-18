#include "simulation_handler.h"
#include <exception>
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

    QString simulationType = data.value("type").toString("dynamic");
    if (simulationType == "static") {
      m_trainSimulation->simulateStaticTrainMovement();
    } else {
      m_trainSimulation->simulateDynamicTrainMovement();
    }
    response["status"] = "success";
    response["message"] = "Simulation completed";
    response["simulationType"] = simulationType;

    QJsonObject summary;
    summary["maxSpeed"] = m_trainSimulation->getMaxSpeed();
    summary["distanceTravelled"] = m_trainSimulation->getDistanceTravelled();

    if (simulationType == "static") {
      summary["distanceOnBraking"] =
          m_trainSimulation->m_simulationTrackHandler->calculateBrakingTrack(
              m_context.movingData->v_limit);
      summary["distanceOnEmergencyBraking"] =
          m_trainSimulation->m_simulationTrackHandler
              ->calculateBrakingEmergencyTrack();
    }

    summary["maxTractionEffort"] = m_trainSimulation->getMaxTractionEffort();
    summary["adhesion"] = m_trainSimulation->getAdhesion();

    summary["maxCatenaryPower"] = m_trainSimulation->getMaxCatenaryPower();
    summary["maxVvvfPower"] = m_trainSimulation->getMaxVvvfPower();
    summary["maxCatenaryCurrent"] = m_trainSimulation->getMaxCatenaryCurrent();
    summary["maxVvvfCurrent"] = m_trainSimulation->getMaxVvvfCurrent();
    summary["maxCurrentTime"] = m_trainSimulation->getMaxCurrTime();
    summary["maxPowerTime"] = m_trainSimulation->getMaxPowTime();

    summary["maxEnergyConsumption"] =
        m_trainSimulation->getMaxEnergyConsumption();
    summary["maxEnergyPowering"] = m_trainSimulation->getMaxEnergyPowering();
    summary["maxEnergyRegen"] = m_trainSimulation->getMaxEnergyRegen();
    summary["maxEnergyAps"] = m_trainSimulation->getMaxEnergyAps();

    response["summary"] = summary;
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
      point["odos"] = simulationDatas->odos[i];
      point["brakingDistances"] = simulationDatas->brakingDistances[i];
      point["slopes"] = simulationDatas->slopes[i];
      point["radiuses"] = simulationDatas->radiuses[i];
      point["speeds"] = simulationDatas->trainSpeeds[i];
      point["speedLimits"] = simulationDatas->speedLimits[i];
      point["speedsSi"] = simulationDatas->trainSpeedsSi[i];
      point["accelerations"] = simulationDatas->accelerations[i];
      point["accelerationsSi"] = simulationDatas->accelerationsSi[i];
      point["motorForce"] = simulationDatas->motorForce[i];
      point["motorResistance"] = simulationDatas->motorResistance[i];
      point["totalResistance"] = simulationDatas->totalResistance[i];
      point["tractionForcePerMotor"] =
          simulationDatas->tractionForcePerMotor[i];
      point["resistancePerMotor"] = simulationDatas->resistancePerMotor[i];
      point["torque"] = simulationDatas->torque[i];
      point["rpm"] = simulationDatas->rpm[i];
      point["powerWheel"] = simulationDatas->powerWheel[i];
      point["powerMotorOut"] = simulationDatas->powerMotorOut[i];
      point["powerMotorIn"] = simulationDatas->powerMotorIn[i];
      point["vvvfPowers"] = simulationDatas->vvvfPowers[i];
      point["vvvfCurrents"] = simulationDatas->vvvfCurrents[i];
      point["catenaryPowers"] = simulationDatas->catenaryPowers[i];
      point["catenaryCurrents"] = simulationDatas->catenaryCurrents[i];
      point["energyConsumptions"] = simulationDatas->energyConsumptions[i];
      point["energyPowerings"] = simulationDatas->energyPowerings[i];
      point["energyRegenerations"] = simulationDatas->energyRegenerations[i];
      point["energyAps"] = simulationDatas->energyAps[i];
      point["energyCatenaries"] = simulationDatas->energyCatenaries[i];

      // TODO : Add simulation type to apply this condition
      // if (simulationType == "static") {
      //           point["motorResistancesZero"] =
      //           simulationDatas->motorResistancesZero[i];
      //           point["motorResistancesFive"] =
      //           simulationDatas->motorResistancesFive[i];
      //           point["motorResistancesTen"] =
      //           simulationDatas->motorResistancesTen[i];
      //           point["motorResistancesTwentyFive"] =
      //           simulationDatas->motorResistancesTwentyFive[i];
      // }

      resultsArray.append(point);
    }

    response["status"] = "success";
    response["results"] = resultsArray;
    response["totalPoints"] = dataSize;

    // Add track distance table
    QJsonObject trackDistanceTable;
    QJsonArray normalBraking;
    QJsonArray emergencyBraking;

    normalBraking.append(
        m_trainSimulation->m_simulationTrackHandler
            ->calculateNormalSimulationTrack(m_context.movingData->v_limit));
    normalBraking.append(
        m_trainSimulation->m_simulationTrackHandler
            ->calculateDelaySimulationTrack(m_context.movingData->v_limit));
    normalBraking.append(
        m_trainSimulation->m_simulationTrackHandler
            ->calculateSafetySimulationTrack(m_context.movingData->v_limit));
    emergencyBraking.append(m_trainSimulation->m_simulationTrackHandler
                                ->calculateEmergencyNormalSimulationTrack());
    emergencyBraking.append(m_trainSimulation->m_simulationTrackHandler
                                ->calculateEmergencyDelaySimulationTrack());
    emergencyBraking.append(m_trainSimulation->m_simulationTrackHandler
                                ->calculateEmergencySafetySimulationTrack());

    // double vLimit = m_context.stationData->stat_v_limit;

    // normalBraking.append(m_trainSimulation->m_simulationTrackHandler
    //                          ->calculateTrackDistanceNormal(vLimit));
    // normalBraking.append(m_trainSimulation->m_simulationTrackHandler
    //                          ->calculateTrackDistanceDelay(vLimit));
    // normalBraking.append(m_trainSimulation->m_simulationTrackHandler
    //                          ->calculateTrackDistanceSafety(vLimit));

    trackDistanceTable["normalBraking"] = normalBraking;
    trackDistanceTable["emergencyBraking"] = emergencyBraking;
    trackDistanceTable["headers"] =
        QJsonArray({"Normal Braking", "Emergency Braking"});
    trackDistanceTable["labels"] =
        QJsonArray({"Normal", "Delay 3s", "Safety 20%"});

    response["trackDistanceTable"] = trackDistanceTable;

  } catch (const std::exception &e) {
    response["status"] = "error";
    response["message"] = QString("Error: %1").arg(e.what());
    return QHttpServerResponse(
        QJsonDocument(response).toJson(),
        QHttpServerResponse::StatusCode::InternalServerError);
  }

  return QHttpServerResponse(QJsonDocument(response).toJson(),
                             QHttpServerResponse::StatusCode::Ok);
}

// #include "simulation_handler.h"

// SimulationHandler::SimulationHandler(AppContext &context, QObject *parent)
//     : QObject{parent}, m_context(context)
// {
//     m_trainSimulation = new TrainSimulationHandler(context, this);
// }

// QHttpServerResponse SimulationHandler::handleStartSimulation(const
// QJsonObject &data)
// {
//     QJsonObject response;

//     try
//     {
//         // Check if simulation handler is initialized
//         if (!m_trainSimulation)
//         {
//             response["status"] = "error";
//             response["message"] = "Simulation handler not initialized";
//             return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                        QHttpServerResponse::StatusCode::InternalServerError);
//         }

//         // Validate CSV variables before starting simulation
//         if (!m_trainSimulation->validateCsvVariables())
//         {
//             response["status"] = "error";
//             response["message"] = "CSV variables validation failed. Check
//             console for warnings."; return
//             QHttpServerResponse(QJsonDocument(response).toJson(),
//                                        QHttpServerResponse::StatusCode::BadRequest);
//         }

//         // Determine simulation type from request data
//         QString simulationType = data.value("type").toString("dynamic"); //
//         default to dynamic

//         qDebug() << "🚀 Starting" << simulationType << "simulation...";

//         // Clear previous warnings
//         m_trainSimulation->clearWarnings();

//         // Trigger the appropriate simulation using Qt signals/slots
//         if (simulationType == "static")
//         {
//             // Call static simulation (slot will be executed)
//             m_trainSimulation->simulateStaticTrainMovement();
//         }
//         else
//         {
//             // Call dynamic simulation (slot will be executed)
//             m_trainSimulation->simulateDynamicTrainMovement();
//         }

//         // Note: The actual simulation runs synchronously in the slot
//         // In Qt GUI apps, you'd emit signals and track progress
//         // For HTTP API, we can return success immediately after slot
//         execution

//         response["status"] = "success";
//         response["message"] = "Simulation completed successfully";
//         response["simulationType"] = simulationType;

//         // Add simulation summary
//         QJsonObject summary;
//         summary["maxSpeed"] = m_trainSimulation->getMaxSpeed();
//         summary["distanceTravelled"] =
//         m_trainSimulation->getDistanceTravelled(); summary["maxVvvfPower"]
//         = m_trainSimulation->getMaxVvvfPower(); summary["maxCatenaryPower"]
//         = m_trainSimulation->getMaxCatenaryPower();
//         summary["maxEnergyConsumption"] =
//         m_trainSimulation->getMaxEnergyConsumption();

//         response["summary"] = summary;

//         qDebug() << "✅ Simulation completed successfully";
//     }
//     catch (const std::exception &e)
//     {
//         qCritical() << "💥 Exception in handleStartSimulation:" << e.what();
//         response["status"] = "error";
//         response["message"] = QString("Simulation error: %1").arg(e.what());
//         return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                    QHttpServerResponse::StatusCode::InternalServerError);
//     }
//     catch (...)
//     {
//         qCritical() << "💥 Unknown exception in handleStartSimulation";
//         response["status"] = "error";
//         response["message"] = "Unknown error during simulation";
//         return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                    QHttpServerResponse::StatusCode::InternalServerError);
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse SimulationHandler::handleGetSimulationStatus()
// {
//     QJsonObject response;
//     response["status"] = "success";

//     if (!m_trainSimulation)
//     {
//         response["isRunning"] = false;
//         response["simulationStatus"] = "unavailable";
//         response["message"] = "Simulation handler not initialized";
//     }
//     else
//     {
//         // Since simulation runs synchronously, it's never "running" when
//         status is checked
//         // It's either completed or idle
//         response["isRunning"] = false;

//         double maxSpeed = m_trainSimulation->getMaxSpeed();

//         if (maxSpeed > 0)
//         {
//             response["simulationStatus"] = "completed";
//             response["hasResults"] = true;
//         }
//         else
//         {
//             response["simulationStatus"] = "idle";
//             response["hasResults"] = false;
//         }

//         // Add current simulation metrics
//         QJsonObject metrics;
//         metrics["maxSpeed"] = maxSpeed;
//         metrics["distanceTravelled"] =
//         m_trainSimulation->getDistanceTravelled(); metrics["maxVvvfPower"]
//         = m_trainSimulation->getMaxVvvfPower(); metrics["adhesion"] =
//         m_trainSimulation->getAdhesion();

//         response["metrics"] = metrics;
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse SimulationHandler::handleGetSimulationResults()
// {
//     QJsonObject response;

//     if (!m_trainSimulation || !m_context.simulationDatas)
//     {
//         response["status"] = "error";
//         response["message"] = "Simulation data not available";
//         return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                    QHttpServerResponse::StatusCode::InternalServerError);
//     }

//     try
//     {
//         // Get simulation data from context
//         const SimulationDatas &simData = *m_context.simulationDatas;

//         // Build results array (limit to prevent huge payloads)
//         QJsonArray resultsArray;
//         int maxPoints = 1000; // Limit data points for API response
//         int dataSize = simData.trainSpeeds.size();
//         int step = (dataSize > maxPoints) ? (dataSize / maxPoints) : 1;

//         for (int i = 0; i < dataSize; i += step)
//         {
//             QJsonObject point;
//             point["time"] = simData.time[i];
//             point["speed"] = simData.trainSpeeds[i];
//             point["speedSI"] = simData.trainSpeedsSi[i];
//             point["distance"] = simData.distance[i];
//             point["acceleration"] = simData.accelerations[i];
//             point["vvvfPower"] = simData.vvvfPowers[i];
//             point["catenaryPower"] = simData.catenaryPowers[i];
//             point["vvvfCurrent"] = simData.vvvfCurrents[i];
//             point["catenaryCurrent"] = simData.catenaryCurrents[i];
//             point["tractiveEffort"] = simData.tractionEfforts[i];
//             point["totalResistance"] = simData.totalResistance[i];
//             point["phase"] = simData.phase[i];

//             resultsArray.append(point);
//         }

//         response["status"] = "success";
//         response["results"] = resultsArray;
//         response["totalPoints"] = dataSize;
//         response["returnedPoints"] = resultsArray.size();

//         // Add summary statistics
//         QJsonObject summary;
//         summary["maxSpeed"] = m_trainSimulation->getMaxSpeed();
//         summary["maxVvvfPower"] = m_trainSimulation->getMaxVvvfPower();
//         summary["maxCatenaryPower"] =
//         m_trainSimulation->getMaxCatenaryPower(); summary["maxVvvfCurrent"]
//         = m_trainSimulation->getMaxVvvfCurrent();
//         summary["maxCatenaryCurrent"] =
//         m_trainSimulation->getMaxCatenaryCurrent();
//         summary["maxTractionEffort"] =
//         m_trainSimulation->getMaxTractionEffort();
//         summary["distanceTravelled"] =
//         m_trainSimulation->getDistanceTravelled();
//         summary["maxEnergyConsumption"] =
//         m_trainSimulation->getMaxEnergyConsumption();
//         summary["maxEnergyRegen"] = m_trainSimulation->getMaxEnergyRegen();
//         summary["adhesion"] = m_trainSimulation->getAdhesion();

//         response["summary"] = summary;

//         // Add track distance table for static simulation
//         QJsonObject trackDistanceTable;
//         QJsonArray normalBraking;
//         QJsonArray emergencyBraking;

//         // Use the track speed limit for table calculation
//         double vLimit = m_context.stationData->stat_v_limit;

//         // Use dedicated track distance methods (without simulation's
//         distanceTotal)
//         normalBraking.append(m_trainSimulation->m_simulationTrackHandler->calculateTrackDistanceNormal(vLimit));
//         normalBraking.append(m_trainSimulation->m_simulationTrackHandler->calculateTrackDistanceDelay(vLimit));
//         normalBraking.append(m_trainSimulation->m_simulationTrackHandler->calculateTrackDistanceSafety(vLimit));

//         // Emergency braking still uses simulation methods (requires actual
//         simulation data)
//         emergencyBraking.append(m_trainSimulation->m_simulationTrackHandler->calculateEmergencyNormalSimulationTrack());
//         emergencyBraking.append(m_trainSimulation->m_simulationTrackHandler->calculateEmergencyDelaySimulationTrack());
//         emergencyBraking.append(m_trainSimulation->m_simulationTrackHandler->calculateEmergencySafetySimulationTrack());

//         trackDistanceTable["normalBraking"] = normalBraking;
//         trackDistanceTable["emergencyBraking"] = emergencyBraking;
//         trackDistanceTable["labels"] = QJsonArray({"Normal", "Delay 3s",
//         "Safety 20%"});

//         response["trackDistanceTable"] = trackDistanceTable;

//         qDebug() << "� Track Distance Table:" << normalBraking[0].toDouble()
//         << normalBraking[1].toDouble() << normalBraking[2].toDouble();
//         qDebug() << "�📊 Returned" << resultsArray.size() << "simulation data
//         points";
//     }
//     catch (const std::exception &e)
//     {
//         qCritical() << "💥 Exception in handleGetSimulationResults:" <<
//         e.what(); response["status"] = "error"; response["message"] =
//         QString("Error retrieving results: %1").arg(e.what()); return
//         QHttpServerResponse(QJsonDocument(response).toJson(),
//                                    QHttpServerResponse::StatusCode::InternalServerError);
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }