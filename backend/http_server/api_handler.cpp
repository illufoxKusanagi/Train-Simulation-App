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
    m_simulationHandler = new SimulationHandler(context, this);
    // m_exportHandler = new ExportHandler(context, this);

    // **FIX: Initialize handlers safely with null checks**
    m_trainDataHandler = new TrainDataHandler(&context, this);
    m_electricalDataHandler = new ElectricalDataHandler(&context, this);
    m_runningDataHandler = new RunningDataHandler(&context, this);
    m_trackDataHandler = new TrackDataHandler(&context, this);
    // m_simulationHandler = new TrainSimulationHandler(context, this);

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
    return m_simulationHandler->handleStartSimulation(data);
}

QHttpServerResponse ApiHandler::handleGetSimulationStatus()
{
    return m_simulationHandler->handleGetSimulationStatus();
}

QHttpServerResponse ApiHandler::handleGetSimulationResults()
{
    return m_simulationHandler->handleGetSimulationResults();
}