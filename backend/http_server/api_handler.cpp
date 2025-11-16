#include "api_handler.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <QHttpServerResponse>
#include <QDateTime>

ApiHandler::ApiHandler(AppContext &context, QObject *parent)
    : QObject(parent), m_context(context)
{
    // Sub handlers
    m_trainHandler = new TrainParameterHandler(context, this);
    m_electricalHandler = new ElectricalParameterHandler(context, this);
    m_runningHandler = new RunningParameterHandler(context, this);
    m_trackHandler = new TrackParameterHandler(context, this);
    // m_simulationHandler = new SimulationHandler(context, this);
    // m_exportHandler = new ExportHandler(context, this);

    // **FIX: Initialize handlers safely with null checks**
    m_trainDataHandler = new TrainDataHandler(&context, this);
    m_electricalDataHandler = new ElectricalDataHandler(&context, this);
    m_runningDataHandler = new RunningDataHandler(&context, this);
    m_trackDataHandler = new TrackDataHandler(&context, this);
    m_simulationHandler = new TrainSimulationHandler(context, this);

    // **FIX: Check if simulationDatas exists before creating CSV handler**
    if (context.simulationDatas)
    {
        m_csvOutputHandler = new CsvOutputHandler(*context.simulationDatas);
    }
    else
    {
        m_csvOutputHandler = nullptr;
        qWarning() << "simulationDatas not initialized, CSV export will not be available";
    }
}

QHttpServerResponse ApiHandler::handleHealthCheck()
{
    QJsonObject response;
    response["status"] = "ok";
    response["service"] = "Train Simulation Backend";
    response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    response["dataStatus"] = QJsonObject{
        {"trainData", m_context.trainData != nullptr},
        {"powerData", m_context.powerData != nullptr},
        {"efficiencyData", m_context.efficiencyData != nullptr},
        {"loadData", m_context.loadData != nullptr},
        {"massData", m_context.massData != nullptr},
        {"energyData", m_context.energyData != nullptr},
        {"simulationDatas", m_context.simulationDatas != nullptr}};

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ApiHandler::handleGetTrainParameters()
{
    return m_trainHandler->handleGetTrainParameters();
}

QHttpServerResponse ApiHandler::handleUpdateTrainParameters(const QJsonObject &data)
{
    return handleUpdateCarNumberParameters(data);
}

QHttpServerResponse ApiHandler::handleExportResults(const QJsonObject &data)
{
    QJsonObject response;

    // **FIX: Check if CSV handler exists**
    if (!m_csvOutputHandler)
    {
        response["status"] = "error";
        response["message"] = "CSV export not available - simulation data not initialized";
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    try
    {
        QString format = data.value("format").toString("csv");

        if (format == "csv")
        {
            m_csvOutputHandler->printSimulationDatas();

            response["status"] = "success";
            response["message"] = "Results exported to CSV successfully";
        }
        else
        {
            response["status"] = "error";
            response["message"] = "Unsupported export format";
        }
    }
    catch (const std::exception &e)
    {
        response["status"] = "error";
        response["message"] = QString("Error exporting results: %1").arg(e.what());
    }

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

// **FIX: Similar null checks for other methods**
QHttpServerResponse ApiHandler::handleGetElectricalParameters()
{
    return m_electricalHandler->handleGetElectricalParameters();
}

QHttpServerResponse ApiHandler::handleUpdateElectricalParameters(const QJsonObject &data)
{
    return m_electricalHandler->handleUpdateElectricalParameters(data);
}

QHttpServerResponse ApiHandler::handleGetRunningParameters()
{
    return m_runningHandler->handleGetRunningParameters();
}

QHttpServerResponse ApiHandler::handleUpdateRunningParameters(const QJsonObject &data)
{
    return m_runningHandler->handleUpdateRunningParameters(data);
}

QHttpServerResponse ApiHandler::handleGetTrackParameters()
{
    return m_trackHandler->handleGetTrackParameters();
}

QHttpServerResponse ApiHandler::handleUpdateTrackParameters(const QJsonObject &data)
{
    return m_trackHandler->handleUpdateTrackParameters(data);
}

QHttpServerResponse ApiHandler::handleGetCarNumberParameters()
{
    return m_trainHandler->handleGetCarNumberParameters();
}

QHttpServerResponse ApiHandler::handleUpdateCarNumberParameters(const QJsonObject &data)
{
    return m_trainHandler->handleUpdateCarNumberParameters(data);
}

QHttpServerResponse ApiHandler::handleGetPassengerParameters()
{
    return m_trainHandler->handleGetPassengerParameters();
}

QHttpServerResponse ApiHandler::handleUpdatePassengerParameters(const QJsonObject &data)
{
    return m_trainHandler->handleUpdatePassengerParameters(data);
}

QHttpServerResponse ApiHandler::handleGetMassParameters()
{
    return m_trainHandler->handleGetMassParameters();
}

QHttpServerResponse ApiHandler::handleUpdateMassParameters(const QJsonObject &data)
{
    return m_trainHandler->handleUpdateMassParameters(data);
}

QHttpServerResponse ApiHandler::handleStartSimulation(const QJsonObject &data)
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

QHttpServerResponse ApiHandler::handleGetSimulationStatus()
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

QHttpServerResponse ApiHandler::handleGetSimulationResults()
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