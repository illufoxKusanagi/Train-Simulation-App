#include "simulation_handler.h"

SimulationHandler::SimulationHandler(AppContext &context, QObject *parent)
    : QObject{parent}, m_context(context)
{
    m_simulationHandler = new TrainSimulationHandler(context, this);
}

QHttpServerResponse SimulationHandler::handleStartSimulation(const QJsonObject &data)
{
    QJsonObject response;

    try
    {
        // Check if simulation handler is initialized
        if (!m_simulationHandler)
        {
            response["status"] = "error";
            response["message"] = "Simulation handler not initialized";
            return QHttpServerResponse(QJsonDocument(response).toJson(),
                                       QHttpServerResponse::StatusCode::InternalServerError);
        }

        // Validate CSV variables before starting simulation
        if (!m_simulationHandler->validateCsvVariables())
        {
            response["status"] = "error";
            response["message"] = "CSV variables validation failed. Check console for warnings.";
            return QHttpServerResponse(QJsonDocument(response).toJson(),
                                       QHttpServerResponse::StatusCode::BadRequest);
        }

        // Determine simulation type from request data
        QString simulationType = data.value("type").toString("dynamic"); // default to dynamic

        qDebug() << "🚀 Starting" << simulationType << "simulation...";

        // Clear previous warnings
        m_simulationHandler->clearWarnings();

        // Trigger the appropriate simulation using Qt signals/slots
        if (simulationType == "static")
        {
            // Call static simulation (slot will be executed)
            m_simulationHandler->simulateStaticTrainMovement();
        }
        else
        {
            // Call dynamic simulation (slot will be executed)
            m_simulationHandler->simulateDynamicTrainMovement();
        }

        // Note: The actual simulation runs synchronously in the slot
        // In Qt GUI apps, you'd emit signals and track progress
        // For HTTP API, we can return success immediately after slot execution

        response["status"] = "success";
        response["message"] = "Simulation completed successfully";
        response["simulationType"] = simulationType;

        // Add simulation summary
        QJsonObject summary;
        summary["maxSpeed"] = m_simulationHandler->getMaxSpeed();
        summary["distanceTravelled"] = m_simulationHandler->getDistanceTravelled();
        summary["maxVvvfPower"] = m_simulationHandler->getMaxVvvfPower();
        summary["maxCatenaryPower"] = m_simulationHandler->getMaxCatenaryPower();
        summary["maxEnergyConsumption"] = m_simulationHandler->getMaxEnergyConsumption();

        response["summary"] = summary;

        qDebug() << "✅ Simulation completed successfully";
    }
    catch (const std::exception &e)
    {
        qCritical() << "💥 Exception in handleStartSimulation:" << e.what();
        response["status"] = "error";
        response["message"] = QString("Simulation error: %1").arg(e.what());
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }
    catch (...)
    {
        qCritical() << "💥 Unknown exception in handleStartSimulation";
        response["status"] = "error";
        response["message"] = "Unknown error during simulation";
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse SimulationHandler::handleGetSimulationStatus()
{
    QJsonObject response;
    response["status"] = "success";

    if (!m_simulationHandler)
    {
        response["isRunning"] = false;
        response["simulationStatus"] = "unavailable";
        response["message"] = "Simulation handler not initialized";
    }
    else
    {
        // Since simulation runs synchronously, it's never "running" when status is checked
        // It's either completed or idle
        response["isRunning"] = false;

        double maxSpeed = m_simulationHandler->getMaxSpeed();

        if (maxSpeed > 0)
        {
            response["simulationStatus"] = "completed";
            response["hasResults"] = true;
        }
        else
        {
            response["simulationStatus"] = "idle";
            response["hasResults"] = false;
        }

        // Add current simulation metrics
        QJsonObject metrics;
        metrics["maxSpeed"] = maxSpeed;
        metrics["distanceTravelled"] = m_simulationHandler->getDistanceTravelled();
        metrics["maxVvvfPower"] = m_simulationHandler->getMaxVvvfPower();
        metrics["adhesion"] = m_simulationHandler->getAdhesion();

        response["metrics"] = metrics;
    }

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse SimulationHandler::handleGetSimulationResults()
{
    QJsonObject response;

    if (!m_simulationHandler || !m_context.simulationDatas)
    {
        response["status"] = "error";
        response["message"] = "Simulation data not available";
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    try
    {
        // Get simulation data from context
        const SimulationDatas &simData = *m_context.simulationDatas;

        // Build results array (limit to prevent huge payloads)
        QJsonArray resultsArray;
        int maxPoints = 1000; // Limit data points for API response
        int dataSize = simData.trainSpeeds.size();
        int step = (dataSize > maxPoints) ? (dataSize / maxPoints) : 1;

        for (int i = 0; i < dataSize; i += step)
        {
            QJsonObject point;
            point["time"] = simData.time[i];
            point["speed"] = simData.trainSpeeds[i];
            point["speedSI"] = simData.trainSpeedsSi[i];
            point["distance"] = simData.distance[i];
            point["acceleration"] = simData.accelerations[i];
            point["vvvfPower"] = simData.vvvfPowers[i];
            point["catenaryPower"] = simData.catenaryPowers[i];
            point["vvvfCurrent"] = simData.vvvfCurrents[i];
            point["catenaryCurrent"] = simData.catenaryCurrents[i];
            point["tractiveEffort"] = simData.tractionEfforts[i];
            point["totalResistance"] = simData.totalResistance[i];
            point["phase"] = simData.phase[i];

            resultsArray.append(point);
        }

        response["status"] = "success";
        response["results"] = resultsArray;
        response["totalPoints"] = dataSize;
        response["returnedPoints"] = resultsArray.size();

        // Add summary statistics
        QJsonObject summary;
        summary["maxSpeed"] = m_simulationHandler->getMaxSpeed();
        summary["maxVvvfPower"] = m_simulationHandler->getMaxVvvfPower();
        summary["maxCatenaryPower"] = m_simulationHandler->getMaxCatenaryPower();
        summary["maxTractionEffort"] = m_simulationHandler->getMaxTractionEffort();
        summary["distanceTravelled"] = m_simulationHandler->getDistanceTravelled();
        summary["maxEnergyConsumption"] = m_simulationHandler->getMaxEnergyConsumption();
        summary["maxEnergyRegen"] = m_simulationHandler->getMaxEnergyRegen();
        summary["adhesion"] = m_simulationHandler->getAdhesion();

        response["summary"] = summary;

        qDebug() << "📊 Returned" << resultsArray.size() << "simulation data points";
    }
    catch (const std::exception &e)
    {
        qCritical() << "💥 Exception in handleGetSimulationResults:" << e.what();
        response["status"] = "error";
        response["message"] = QString("Error retrieving results: %1").arg(e.what());
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}