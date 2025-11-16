#include "running_parameter_handler.h"

RunningParameterHandler::RunningParameterHandler(AppContext &context, QObject *parent)
    : QObject{parent}, m_context(context)
{
}

QHttpServerResponse RunningParameterHandler::handleGetRunningParameters()
{
    QJsonObject response;

    if (!m_context.loadData || !m_context.movingData)
    {
        response["status"] = "error";
        response["message"] = "Running data not initialized";
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    QJsonObject runningParams;

    // Load/Passenger data
    runningParams["passengerLoad"] = m_context.loadData->load;
    runningParams["passengerMass"] = m_context.loadData->mass_P;
    runningParams["passengerMassFinal"] = m_context.loadData->mass_P_final;
    runningParams["passengersM1"] = m_context.loadData->n_PM1;
    runningParams["passengersM2"] = m_context.loadData->n_PM2;
    runningParams["passengersTc"] = m_context.loadData->n_PTc;
    runningParams["passengersT1"] = m_context.loadData->n_PT1;
    runningParams["passengersT2"] = m_context.loadData->n_PT2;
    runningParams["passengersT3"] = m_context.loadData->n_PT3;

    // Motion parameters
    runningParams["acceleration"] = m_context.movingData->acc;
    runningParams["deceleration"] = m_context.movingData->decc;
    runningParams["startingAcceleration"] = m_context.movingData->acc_start;
    runningParams["startingDeceleration"] = m_context.movingData->decc_start;
    runningParams["emergencyDeceleration"] = m_context.movingData->decc_emergency;
    runningParams["speedLimit"] = m_context.movingData->v_limit;
    runningParams["coastingSpeedDiff"] = m_context.movingData->v_diffCoast;
    runningParams["stationDistance"] = m_context.movingData->x_station;

    response["runningParameters"] = runningParams;
    response["status"] = "success";

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse RunningParameterHandler::handleUpdateRunningParameters(const QJsonObject &data)
{
    QJsonObject response;

    if (!m_context.loadData || !m_context.movingData)
    {
        response["status"] = "error";
        response["message"] = "Running data not initialized";
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    try
    {
        if (data.contains("runningParameters"))
        {
            QJsonObject runningParams = data["runningParameters"].toObject();
            qDebug() << "📝 Updating running parameters:" << runningParams;

            // Update load data
            if (runningParams.contains("passengerLoad") || runningParams.contains("load"))
            {
                m_context.loadData->load = runningParams.contains("load") ? runningParams["load"].toDouble() : runningParams["passengerLoad"].toDouble();
            }
            if (runningParams.contains("passengerMass") || runningParams.contains("massPerPassenger"))
            {
                m_context.loadData->mass_P = runningParams.contains("massPerPassenger") ? runningParams["massPerPassenger"].toDouble() : runningParams["passengerMass"].toDouble();
            }

            // Support both naming conventions: passengersM1 and passengersPerM1
            if (runningParams.contains("passengersM1") || runningParams.contains("passengersPerM1"))
            {
                m_context.loadData->n_PM1 = runningParams.contains("passengersPerM1") ? runningParams["passengersPerM1"].toDouble() : runningParams["passengersM1"].toDouble();
            }
            if (runningParams.contains("passengersM2") || runningParams.contains("passengersPerM2"))
            {
                m_context.loadData->n_PM2 = runningParams.contains("passengersPerM2") ? runningParams["passengersPerM2"].toDouble() : runningParams["passengersM2"].toDouble();
            }
            if (runningParams.contains("passengersTc") || runningParams.contains("passengersPerTc"))
            {
                m_context.loadData->n_PTc = runningParams.contains("passengersPerTc") ? runningParams["passengersPerTc"].toDouble() : runningParams["passengersTc"].toDouble();
            }
            if (runningParams.contains("passengersT1") || runningParams.contains("passengersPerT1"))
            {
                m_context.loadData->n_PT1 = runningParams.contains("passengersPerT1") ? runningParams["passengersPerT1"].toDouble() : runningParams["passengersT1"].toDouble();
            }
            if (runningParams.contains("passengersT2") || runningParams.contains("passengersPerT2"))
            {
                m_context.loadData->n_PT2 = runningParams.contains("passengersPerT2") ? runningParams["passengersPerT2"].toDouble() : runningParams["passengersT2"].toDouble();
            }
            if (runningParams.contains("passengersT3") || runningParams.contains("passengersPerT3"))
            {
                m_context.loadData->n_PT3 = runningParams.contains("passengersPerT3") ? runningParams["passengersPerT3"].toDouble() : runningParams["passengersT3"].toDouble();
            }

            // Update motion parameters
            if (runningParams.contains("acceleration"))
            {
                m_context.movingData->acc = runningParams["acceleration"].toDouble();
                m_context.movingData->acc_si = m_context.movingData->acc / 3.6; // Convert to m/s²
            }
            if (runningParams.contains("deceleration"))
            {
                m_context.movingData->decc = runningParams["deceleration"].toDouble();
                m_context.movingData->decc_si = m_context.movingData->decc / 3.6;
            }
            if (runningParams.contains("startingAcceleration"))
            {
                m_context.movingData->acc_start = runningParams["startingAcceleration"].toDouble();
            }
            if (runningParams.contains("startingDeceleration"))
            {
                m_context.movingData->decc_start = runningParams["startingDeceleration"].toDouble();
            }
            if (runningParams.contains("emergencyDeceleration"))
            {
                m_context.movingData->decc_emergency = runningParams["emergencyDeceleration"].toDouble();
            }
            if (runningParams.contains("speedLimit"))
            {
                m_context.movingData->v_limit = runningParams["speedLimit"].toDouble();
            }
            if (runningParams.contains("coastingSpeedDiff"))
            {
                m_context.movingData->v_diffCoast = runningParams["coastingSpeedDiff"].toDouble();
            }
            if (runningParams.contains("stationDistance"))
            {
                m_context.movingData->x_station = runningParams["stationDistance"].toDouble();
            }

            response["status"] = "success";
            response["message"] = "Running parameters updated successfully";
            qDebug() << "✅ Running parameters updated successfully";
        }
        else
        {
            response["status"] = "error";
            response["message"] = "No running parameters provided";
        }
    }
    catch (const std::exception &e)
    {
        qDebug() << "💥 Exception in handleUpdateRunningParameters:" << e.what();
        response["status"] = "error";
        response["message"] = QString("Error updating parameters: %1").arg(e.what());
    }

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}
