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
    QJsonObject response;

    // **FIX: Add null check and initialize data if needed**
    if (!m_context.trainData)
    {
        response["status"] = "error";
        response["message"] = "Train data not initialized";
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    QJsonObject trainParams;
    trainParams["tractionMotors"] = m_context.trainData->n_tm;
    trainParams["axles"] = m_context.trainData->n_axle;
    trainParams["cars"] = m_context.trainData->n_car;
    trainParams["gearRatio"] = m_context.trainData->gearRatio;
    trainParams["wheelDiameter"] = m_context.trainData->wheel;
    trainParams["trainsetLength"] = m_context.trainData->trainsetLength;
    trainParams["motorCars1"] = m_context.trainData->n_M1;
    trainParams["motorCars2"] = m_context.trainData->n_M2;
    trainParams["trailerCabCars"] = m_context.trainData->n_Tc;
    trainParams["trailerCars1"] = m_context.trainData->n_T1;
    trainParams["trailerCars2"] = m_context.trainData->n_T2;
    trainParams["trailerCars3"] = m_context.trainData->n_T3;
    trainParams["motorCars1Disabled"] = m_context.trainData->n_M1_disabled;
    trainParams["motorCars2Disabled"] = m_context.trainData->n_M2_disabled;

    response["trainParameters"] = trainParams;
    response["status"] = "success";

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ApiHandler::handleUpdateTrainParameters(const QJsonObject &data)
{
    QJsonObject response;

    // **FIX: Add null check**
    if (!m_context.trainData)
    {
        response["status"] = "error";
        response["message"] = "Train data not initialized";
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    try
    {
        if (data.contains("trainParameters"))
        {
            QJsonObject trainParams = data["trainParameters"].toObject();
            qDebug() << "📝 Updating train parameters:" << trainParams;

            // Update train data using actual TrainData variables
            if (trainParams.contains("tractionMotors"))
            {
                m_context.trainData->n_tm = trainParams["tractionMotors"].toDouble();
                qDebug() << "Updated n_tm to:" << m_context.trainData->n_tm;
            }
            if (trainParams.contains("axles"))
            {
                m_context.trainData->n_axle = trainParams["axles"].toDouble();
                qDebug() << "Updated n_axle to:" << m_context.trainData->n_axle;
            }
            if (trainParams.contains("cars"))
            {
                m_context.trainData->n_car = trainParams["cars"].toDouble();
                qDebug() << "Updated n_car to:" << m_context.trainData->n_car;
            }
            if (trainParams.contains("gearRatio"))
            {
                m_context.trainData->gearRatio = trainParams["gearRatio"].toDouble();
                qDebug() << "Updated gearRatio to:" << m_context.trainData->gearRatio;
            }
            if (trainParams.contains("wheelDiameter"))
            {
                m_context.trainData->wheel = trainParams["wheelDiameter"].toDouble();
                qDebug() << "Updated wheel to:" << m_context.trainData->wheel;
            }

            response["status"] = "success";
            response["message"] = "Train parameters updated successfully";
            qDebug() << "✅ Train parameters updated successfully";
        }
        else
        {
            qDebug() << "❌ No train parameters provided in request";
            response["status"] = "error";
            response["message"] = "No train parameters provided";
        }
    }
    catch (const std::exception &e)
    {
        qDebug() << "💥 Exception in handleUpdateTrainParameters:" << e.what();
        response["status"] = "error";
        response["message"] = QString("Error updating parameters: %1").arg(e.what());
    }
    catch (...)
    {
        qDebug() << "💥 Unknown exception in handleUpdateTrainParameters";
        response["status"] = "error";
        response["message"] = "Unknown error updating parameters";
    }

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

// **FIX: Similar null checks for other methods**
QHttpServerResponse ApiHandler::handleGetElectricalParameters()
{
    QJsonObject response;

    if (!m_context.powerData || !m_context.efficiencyData)
    {
        response["status"] = "error";
        response["message"] = "Electrical data not initialized";
        return QHttpServerResponse(QJsonDocument(response).toJson(),
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

    electricalParams["staticGearEfficiency"] = m_context.efficiencyData->stat_eff_gear;
    electricalParams["staticMotorEfficiency"] = m_context.efficiencyData->stat_eff_motor;
    electricalParams["staticVvvfEfficiency"] = m_context.efficiencyData->stat_eff_vvvf;

    response["electricalParameters"] = electricalParams;
    response["status"] = "success";

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

// **Continue with the rest of the methods from your original file, but add null checks**

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

QHttpServerResponse ApiHandler::handleUpdateElectricalParameters(const QJsonObject &data)
{
    QJsonObject response;

    if (!m_context.powerData || !m_context.efficiencyData)
    {
        response["status"] = "error";
        response["message"] = "Electrical data not initialized";
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    try
    {
        if (data.contains("electricalParameters"))
        {
            QJsonObject params = data["electricalParameters"].toObject();

            // Update power data
            if (params.contains("wheelPower"))
            {
                m_context.powerData->p_wheel = params["wheelPower"].toDouble();
            }
            if (params.contains("motorOutPower"))
            {
                m_context.powerData->p_motorOut = params["motorOutPower"].toDouble();
            }
            // Add other parameters as needed...

            response["status"] = "success";
            response["message"] = "Electrical parameters updated successfully";
        }
        else
        {
            response["status"] = "error";
            response["message"] = "No electrical parameters provided";
        }
    }
    catch (const std::exception &e)
    {
        response["status"] = "error";
        response["message"] = QString("Error updating parameters: %1").arg(e.what());
    }

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ApiHandler::handleGetRunningParameters()
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

QHttpServerResponse ApiHandler::handleUpdateRunningParameters(const QJsonObject &data)
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
            if (runningParams.contains("passengerLoad"))
            {
                m_context.loadData->load = runningParams["passengerLoad"].toDouble();
            }
            if (runningParams.contains("passengerMass"))
            {
                m_context.loadData->mass_P = runningParams["passengerMass"].toDouble();
            }
            if (runningParams.contains("passengersM1"))
            {
                m_context.loadData->n_PM1 = runningParams["passengersM1"].toDouble();
            }
            if (runningParams.contains("passengersM2"))
            {
                m_context.loadData->n_PM2 = runningParams["passengersM2"].toDouble();
            }
            if (runningParams.contains("passengersTc"))
            {
                m_context.loadData->n_PTc = runningParams["passengersTc"].toDouble();
            }
            if (runningParams.contains("passengersT1"))
            {
                m_context.loadData->n_PT1 = runningParams["passengersT1"].toDouble();
            }
            if (runningParams.contains("passengersT2"))
            {
                m_context.loadData->n_PT2 = runningParams["passengersT2"].toDouble();
            }
            if (runningParams.contains("passengersT3"))
            {
                m_context.loadData->n_PT3 = runningParams["passengersT3"].toDouble();
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

QHttpServerResponse ApiHandler::handleGetTrackParameters()
{
    QJsonObject response;

    if (!m_context.stationData)
    {
        response["status"] = "error";
        response["message"] = "Track data not initialized";
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    QJsonObject trackParams;
    trackParams["numberOfStations"] = m_context.stationData->n_station;

    // Note: stationData typically contains arrays/vectors for multiple stations
    // For now, we return basic info. You may need to expand this based on your needs.

    response["trackParameters"] = trackParams;
    response["status"] = "success";

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ApiHandler::handleUpdateTrackParameters(const QJsonObject &data)
{
    QJsonObject response;

    if (!m_context.stationData)
    {
        response["status"] = "error";
        response["message"] = "Track data not initialized";
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    try
    {
        if (data.contains("trackParameters"))
        {
            QJsonObject trackParams = data["trackParameters"].toObject();
            qDebug() << "📝 Updating track parameters:" << trackParams;

            if (trackParams.contains("numberOfStations"))
            {
                m_context.stationData->n_station = trackParams["numberOfStations"].toInt();
            }

            // Add more track parameter updates as needed

            response["status"] = "success";
            response["message"] = "Track parameters updated successfully";
            qDebug() << "✅ Track parameters updated successfully";
        }
        else
        {
            response["status"] = "error";
            response["message"] = "No track parameters provided";
        }
    }
    catch (const std::exception &e)
    {
        qDebug() << "💥 Exception in handleUpdateTrackParameters:" << e.what();
        response["status"] = "error";
        response["message"] = QString("Error updating parameters: %1").arg(e.what());
    }

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ApiHandler::handleGetMassParameters()
{
    QJsonObject response;

    if (!m_context.massData)
    {
        response["status"] = "error";
        response["message"] = "Mass data not initialized";
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    QJsonObject massParams;

    // Car masses (empty)
    massParams["massM1"] = m_context.massData->mass_M1;
    massParams["massM2"] = m_context.massData->mass_M2;
    massParams["massTc"] = m_context.massData->mass_TC;
    massParams["massT1"] = m_context.massData->mass_T1;
    massParams["massT2"] = m_context.massData->mass_T2;
    massParams["massT3"] = m_context.massData->mass_T3;

    // Rotational inertia coefficients
    massParams["rotationalInertiaMotor"] = m_context.massData->i_M;
    massParams["rotationalInertiaTrailer"] = m_context.massData->i_T;

    // Total masses (calculated)
    massParams["totalEmptyMass"] = m_context.massData->mass_totalEmpty;
    massParams["totalLoadedMass"] = m_context.massData->mass_totalLoad;
    massParams["totalInertialMass"] = m_context.massData->mass_totalInertial;

    response["massParameters"] = massParams;
    response["status"] = "success";

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
}

QHttpServerResponse ApiHandler::handleUpdateMassParameters(const QJsonObject &data)
{
    QJsonObject response;

    if (!m_context.massData)
    {
        response["status"] = "error";
        response["message"] = "Mass data not initialized";
        return QHttpServerResponse(QJsonDocument(response).toJson(),
                                   QHttpServerResponse::StatusCode::InternalServerError);
    }

    try
    {
        if (data.contains("massParameters"))
        {
            QJsonObject massParams = data["massParameters"].toObject();
            qDebug() << "📝 Updating mass parameters:" << massParams;

            // Update car masses
            if (massParams.contains("massM1"))
            {
                m_context.massData->mass_M1 = massParams["massM1"].toDouble();
            }
            if (massParams.contains("massM2"))
            {
                m_context.massData->mass_M2 = massParams["massM2"].toDouble();
            }
            if (massParams.contains("massTc"))
            {
                m_context.massData->mass_TC = massParams["massTc"].toDouble();
            }
            if (massParams.contains("massT1"))
            {
                m_context.massData->mass_T1 = massParams["massT1"].toDouble();
            }
            if (massParams.contains("massT2"))
            {
                m_context.massData->mass_T2 = massParams["massT2"].toDouble();
            }
            if (massParams.contains("massT3"))
            {
                m_context.massData->mass_T3 = massParams["massT3"].toDouble();
            }

            // Update rotational inertia
            if (massParams.contains("rotationalInertiaMotor"))
            {
                m_context.massData->i_M = massParams["rotationalInertiaMotor"].toDouble();
            }
            if (massParams.contains("rotationalInertiaTrailer"))
            {
                m_context.massData->i_T = massParams["rotationalInertiaTrailer"].toDouble();
            }

            response["status"] = "success";
            response["message"] = "Mass parameters updated successfully";
            qDebug() << "✅ Mass parameters updated successfully";
        }
        else
        {
            response["status"] = "error";
            response["message"] = "No mass parameters provided";
        }
    }
    catch (const std::exception &e)
    {
        qDebug() << "💥 Exception in handleUpdateMassParameters:" << e.what();
        response["status"] = "error";
        response["message"] = QString("Error updating parameters: %1").arg(e.what());
    }

    return QHttpServerResponse(QJsonDocument(response).toJson(),
                               QHttpServerResponse::StatusCode::Ok);
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
        response["simulationStatus"] = "unavailable";
        response["message"] = "Simulation handler not initialized";
    }
    else
    {
        // In a real async implementation, you'd track running state
        // For now, we return metrics that indicate if simulation has run
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

// #include "api_handler.h"
// #include <QJsonDocument>
// #include <QJsonObject>
// #include <QJsonArray>
// #include <QDebug>
// #include <QHttpServerResponse>
// #include <QDateTime>

// // Edited here: Fixed constructor to match actual data handler classes and removed non-existent ones
// ApiHandler::ApiHandler(AppContext &context, QObject *parent)
//     : QObject(parent), m_context(context)
// {
//     // Initialize handlers with context - using actual handler classes from your codebase
//     m_trainDataHandler = new TrainDataHandler(&context, this);
//     m_electricalDataHandler = new ElectricalDataHandler(&context, this);
//     m_runningDataHandler = new RunningDataHandler(&context, this);
//     m_trackDataHandler = new TrackDataHandler(&context, this);
//     m_simulationHandler = new TrainSimulationHandler(context, this);
//     m_csvOutputHandler = new CsvOutputHandler(*context.simulationDatas); // Edited here: Fixed to use simulationDatas not SimulationData
// }

// QHttpServerResponse ApiHandler::handleHealthCheck()
// {
//     QJsonObject response;
//     response["status"] = "ok";
//     response["service"] = "Train Simulation Backend";
//     response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleGetTrainParameters()
// {
//     QJsonObject response;

//     // Edited here: Fixed to use actual TrainData structure variables from your models
//     if (m_context.trainData) {
//         QJsonObject trainParams;
//         trainParams["tractionMotors"] = m_context.trainData->n_tm;
//         trainParams["axles"] = m_context.trainData->n_axle;
//         trainParams["cars"] = m_context.trainData->n_car;
//         trainParams["gearRatio"] = m_context.trainData->gearRatio;
//         trainParams["wheelDiameter"] = m_context.trainData->wheel;
//         trainParams["trainsetLength"] = m_context.trainData->trainsetLength;
//         trainParams["motorCars1"] = m_context.trainData->n_M1;
//         trainParams["motorCars2"] = m_context.trainData->n_M2;
//         trainParams["trailerCabCars"] = m_context.trainData->n_Tc;
//         trainParams["trailerCars1"] = m_context.trainData->n_T1;
//         trainParams["trailerCars2"] = m_context.trainData->n_T2;
//         trainParams["trailerCars3"] = m_context.trainData->n_T3;
//         trainParams["motorCars1Disabled"] = m_context.trainData->n_M1_disabled;
//         trainParams["motorCars2Disabled"] = m_context.trainData->n_M2_disabled;

//         response["trainParameters"] = trainParams;
//         response["status"] = "success";
//     } else {
//         response["status"] = "error";
//         response["message"] = "Train data not initialized";
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleUpdateTrainParameters(const QJsonObject &data)
// {
//     QJsonObject response;

//     try {
//         if (data.contains("trainParameters")) {
//             QJsonObject trainParams = data["trainParameters"].toObject();

//             // Edited here: Update train data using actual TrainData variables
//             if (trainParams.contains("tractionMotors")) {
//                 m_context.trainData->n_tm = trainParams["tractionMotors"].toDouble();
//             }
//             if (trainParams.contains("axles")) {
//                 m_context.trainData->n_axle = trainParams["axles"].toDouble();
//             }
//             if (trainParams.contains("cars")) {
//                 m_context.trainData->n_car = trainParams["cars"].toDouble();
//             }
//             if (trainParams.contains("gearRatio")) {
//                 m_context.trainData->gearRatio = trainParams["gearRatio"].toDouble();
//             }
//             if (trainParams.contains("wheelDiameter")) {
//                 m_context.trainData->wheel = trainParams["wheelDiameter"].toDouble();
//             }
//             if (trainParams.contains("trainsetLength")) {
//                 m_context.trainData->trainsetLength = trainParams["trainsetLength"].toDouble();
//             }
//             if (trainParams.contains("motorCars1")) {
//                 m_context.trainData->n_M1 = trainParams["motorCars1"].toDouble();
//             }
//             if (trainParams.contains("motorCars2")) {
//                 m_context.trainData->n_M2 = trainParams["motorCars2"].toDouble();
//             }
//             if (trainParams.contains("trailerCabCars")) {
//                 m_context.trainData->n_Tc = trainParams["trailerCabCars"].toDouble();
//             }
//             if (trainParams.contains("trailerCars1")) {
//                 m_context.trainData->n_T1 = trainParams["trailerCars1"].toDouble();
//             }
//             if (trainParams.contains("trailerCars2")) {
//                 m_context.trainData->n_T2 = trainParams["trailerCars2"].toDouble();
//             }
//             if (trainParams.contains("trailerCars3")) {
//                 m_context.trainData->n_T3 = trainParams["trailerCars3"].toDouble();
//             }
//             if (trainParams.contains("motorCars1Disabled")) {
//                 m_context.trainData->n_M1_disabled = trainParams["motorCars1Disabled"].toDouble();
//             }
//             if (trainParams.contains("motorCars2Disabled")) {
//                 m_context.trainData->n_M2_disabled = trainParams["motorCars2Disabled"].toDouble();
//             }

//             response["status"] = "success";
//             response["message"] = "Train parameters updated successfully";
//         } else {
//             response["status"] = "error";
//             response["message"] = "No train parameters provided";
//         }
//     } catch (const std::exception &e) {
//         response["status"] = "error";
//         response["message"] = QString("Error updating parameters: %1").arg(e.what());
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleGetElectricalParameters()
// {
//     QJsonObject response;

//     // Edited here: Fixed to use actual powerData and efficiencyData structures from your models
//     if (m_context.powerData && m_context.efficiencyData) {
//         QJsonObject electricalParams;
//         // Using actual variables from PowerData struct
//         electricalParams["wheelPower"] = m_context.powerData->p_wheel;
//         electricalParams["motorOutPower"] = m_context.powerData->p_motorOut;
//         electricalParams["motorInPower"] = m_context.powerData->p_motorIn;
//         electricalParams["vvvfInPower"] = m_context.powerData->p_vvvfIn;
//         electricalParams["catenaryPower"] = m_context.powerData->p_catenary;
//         electricalParams["apsPower"] = m_context.powerData->p_aps;
//         electricalParams["staticApsPower"] = m_context.powerData->stat_p_aps;

//         // Using actual variables from EfficiencyData struct
//         electricalParams["staticGearEfficiency"] = m_context.efficiencyData->stat_eff_gear;
//         electricalParams["staticMotorEfficiency"] = m_context.efficiencyData->stat_eff_motor;
//         electricalParams["staticVvvfEfficiency"] = m_context.efficiencyData->stat_eff_vvvf;

//         response["electricalParameters"] = electricalParams;
//         response["status"] = "success";
//     } else {
//         response["status"] = "error";
//         response["message"] = "Electrical data not initialized";
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleUpdateElectricalParameters(const QJsonObject &data)
// {
//     QJsonObject response;

//     try {
//         if (data.contains("electricalParameters")) {
//             QJsonObject electricalParams = data["electricalParameters"].toObject();

//             // Edited here: Update actual PowerData variables
//             if (electricalParams.contains("wheelPower")) {
//                 m_context.powerData->p_wheel = electricalParams["wheelPower"].toDouble();
//             }
//             if (electricalParams.contains("motorOutPower")) {
//                 m_context.powerData->p_motorOut = electricalParams["motorOutPower"].toDouble();
//             }
//             if (electricalParams.contains("motorInPower")) {
//                 m_context.powerData->p_motorIn = electricalParams["motorInPower"].toDouble();
//             }
//             if (electricalParams.contains("vvvfInPower")) {
//                 m_context.powerData->p_vvvfIn = electricalParams["vvvfInPower"].toDouble();
//             }
//             if (electricalParams.contains("catenaryPower")) {
//                 m_context.powerData->p_catenary = electricalParams["catenaryPower"].toDouble();
//             }
//             if (electricalParams.contains("apsPower")) {
//                 m_context.powerData->p_aps = electricalParams["apsPower"].toDouble();
//             }
//             if (electricalParams.contains("staticApsPower")) {
//                 m_context.powerData->stat_p_aps = electricalParams["staticApsPower"].toDouble();
//             }

//             // Edited here: Update actual EfficiencyData variables
//             if (electricalParams.contains("staticGearEfficiency")) {
//                 m_context.efficiencyData->stat_eff_gear = electricalParams["staticGearEfficiency"].toDouble();
//             }
//             if (electricalParams.contains("staticMotorEfficiency")) {
//                 m_context.efficiencyData->stat_eff_motor = electricalParams["staticMotorEfficiency"].toDouble();
//             }
//             if (electricalParams.contains("staticVvvfEfficiency")) {
//                 m_context.efficiencyData->stat_eff_vvvf = electricalParams["staticVvvfEfficiency"].toDouble();
//             }

//             response["status"] = "success";
//             response["message"] = "Electrical parameters updated successfully";
//         } else {
//             response["status"] = "error";
//             response["message"] = "No electrical parameters provided";
//         }
//     } catch (const std::exception &e) {
//         response["status"] = "error";
//         response["message"] = QString("Error updating electrical parameters: %1").arg(e.what());
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleGetRunningParameters()
// {
//     QJsonObject response;

//     // Edited here: Fixed to use actual LoadData and MassData structures since there's no movingData/resistanceData in models
//     if (m_context.loadData && m_context.massData) {
//         QJsonObject runningParams;
//         // Using actual LoadData variables
//         runningParams["load"] = m_context.loadData->load;
//         runningParams["passengerMass"] = m_context.loadData->mass_P;
//         runningParams["finalPassengerMass"] = m_context.loadData->mass_P_final;
//         runningParams["passengersM1"] = m_context.loadData->n_PM1;
//         runningParams["passengersM2"] = m_context.loadData->n_PM2;
//         runningParams["passengersTc"] = m_context.loadData->n_PTc;
//         runningParams["passengersT1"] = m_context.loadData->n_PT1;
//         runningParams["passengersT2"] = m_context.loadData->n_PT2;
//         runningParams["passengersT3"] = m_context.loadData->n_PT3;

//         // Using actual MassData variables
//         runningParams["totalEmptyMass"] = m_context.massData->mass_totalEmpty;
//         runningParams["totalLoadMass"] = m_context.massData->mass_totalLoad;
//         runningParams["totalInertialMass"] = m_context.massData->mass_totalInertial;

//         response["runningParameters"] = runningParams;
//         response["status"] = "success";
//     } else {
//         response["status"] = "error";
//         response["message"] = "Running data not initialized";
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleUpdateRunningParameters(const QJsonObject &data)
// {
//     QJsonObject response;

//     try {
//         if (data.contains("runningParameters")) {
//             QJsonObject runningParams = data["runningParameters"].toObject();

//             // Edited here: Update LoadData and MassData directly using actual variables
//             if (runningParams.contains("load")) {
//                 m_context.loadData->load = runningParams["load"].toDouble();
//             }
//             if (runningParams.contains("passengerMass")) {
//                 m_context.loadData->mass_P = runningParams["passengerMass"].toDouble();
//             }
//             if (runningParams.contains("finalPassengerMass")) {
//                 m_context.loadData->mass_P_final = runningParams["finalPassengerMass"].toDouble();
//             }
//             if (runningParams.contains("passengersM1")) {
//                 m_context.loadData->n_PM1 = runningParams["passengersM1"].toDouble();
//             }
//             if (runningParams.contains("passengersM2")) {
//                 m_context.loadData->n_PM2 = runningParams["passengersM2"].toDouble();
//             }
//             if (runningParams.contains("passengersTc")) {
//                 m_context.loadData->n_PTc = runningParams["passengersTc"].toDouble();
//             }
//             if (runningParams.contains("passengersT1")) {
//                 m_context.loadData->n_PT1 = runningParams["passengersT1"].toDouble();
//             }
//             if (runningParams.contains("passengersT2")) {
//                 m_context.loadData->n_PT2 = runningParams["passengersT2"].toDouble();
//             }
//             if (runningParams.contains("passengersT3")) {
//                 m_context.loadData->n_PT3 = runningParams["passengersT3"].toDouble();
//             }
//             if (runningParams.contains("totalEmptyMass")) {
//                 m_context.massData->mass_totalEmpty = runningParams["totalEmptyMass"].toDouble();
//             }
//             if (runningParams.contains("totalLoadMass")) {
//                 m_context.massData->mass_totalLoad = runningParams["totalLoadMass"].toDouble();
//             }
//             if (runningParams.contains("totalInertialMass")) {
//                 m_context.massData->mass_totalInertial = runningParams["totalInertialMass"].toDouble();
//             }

//             response["status"] = "success";
//             response["message"] = "Running parameters updated successfully";
//         } else {
//             response["status"] = "error";
//             response["message"] = "No running parameters provided";
//         }
//     } catch (const std::exception &e) {
//         response["status"] = "error";
//         response["message"] = QString("Error updating running parameters: %1").arg(e.what());
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleGetTrackParameters()
// {
//     QJsonObject response;

//     // Edited here: Using actual EnergyData structure since stationData is empty in models
//     if (m_context.energyData) {
//         QJsonObject trackParams;
//         trackParams["staticLineVoltage"] = m_context.energyData->stat_vol_line;
//         trackParams["staticMotorVoltage"] = m_context.energyData->stat_vol_motor;
//         trackParams["motorEnergy"] = m_context.energyData->e_motor;
//         trackParams["powerEnergy"] = m_context.energyData->e_pow;
//         trackParams["regenerativeEnergy"] = m_context.energyData->e_reg;
//         trackParams["apsEnergy"] = m_context.energyData->e_aps;
//         trackParams["catenaryEnergy"] = m_context.energyData->e_catenary;
//         trackParams["catendaryCurrent"] = m_context.energyData->curr_catenary;
//         trackParams["vvvfCurrent"] = m_context.energyData->curr_vvvf;

//         response["trackParameters"] = trackParams;
//         response["status"] = "success";
//     } else {
//         response["status"] = "error";
//         response["message"] = "Track data not initialized";
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleUpdateTrackParameters(const QJsonObject &data)
// {
//     QJsonObject response;

//     try {
//         if (data.contains("trackParameters")) {
//             QJsonObject trackParams = data["trackParameters"].toObject();

//             // Edited here: Update EnergyData using actual variables
//             if (trackParams.contains("staticLineVoltage")) {
//                 m_context.energyData->stat_vol_line = trackParams["staticLineVoltage"].toDouble();
//             }
//             if (trackParams.contains("staticMotorVoltage")) {
//                 m_context.energyData->stat_vol_motor = trackParams["staticMotorVoltage"].toDouble();
//             }
//             if (trackParams.contains("motorEnergy")) {
//                 m_context.energyData->e_motor = trackParams["motorEnergy"].toDouble();
//             }
//             if (trackParams.contains("powerEnergy")) {
//                 m_context.energyData->e_pow = trackParams["powerEnergy"].toDouble();
//             }
//             if (trackParams.contains("regenerativeEnergy")) {
//                 m_context.energyData->e_reg = trackParams["regenerativeEnergy"].toDouble();
//             }
//             if (trackParams.contains("apsEnergy")) {
//                 m_context.energyData->e_aps = trackParams["apsEnergy"].toDouble();
//             }
//             if (trackParams.contains("catenaryEnergy")) {
//                 m_context.energyData->e_catenary = trackParams["catenaryEnergy"].toDouble();
//             }
//             if (trackParams.contains("catendaryCurrent")) {
//                 m_context.energyData->curr_catenary = trackParams["catendaryCurrent"].toDouble();
//             }
//             if (trackParams.contains("vvvfCurrent")) {
//                 m_context.energyData->curr_vvvf = trackParams["vvvfCurrent"].toDouble();
//             }

//             response["status"] = "success";
//             response["message"] = "Track parameters updated successfully";
//         } else {
//             response["status"] = "error";
//             response["message"] = "No track parameters provided";
//         }
//     } catch (const std::exception &e) {
//         response["status"] = "error";
//         response["message"] = QString("Error updating track parameters: %1").arg(e.what());
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleStartSimulation(const QJsonObject &data)
// {
//     QJsonObject response;

//     try {
//         QString simulationType = data.value("type").toString("dynamic");

//         if (simulationType == "dynamic") {
//             // Edited here: Use correct method name from TrainSimulationHandler
//             m_simulationHandler->simulateDynamicTrainMovement();

//             response["status"] = "success";
//             response["message"] = "Dynamic simulation started";
//             response["type"] = "dynamic";
//         } else {
//             response["status"] = "error";
//             response["message"] = "Unknown simulation type";
//         }
//     } catch (const std::exception &e) {
//         response["status"] = "error";
//         response["message"] = QString("Error starting simulation: %1").arg(e.what());
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleGetSimulationStatus()
// {
//     QJsonObject response;

//     // Edited here: Simple status response - you may want to add actual status tracking
//     response["status"] = "success";
//     response["simulationRunning"] = false;
//     response["progress"] = 100.0;
//     response["message"] = "Simulation completed";

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleGetSimulationResults()
// {
//     QJsonObject response;

//     try {
//         // Edited here: Fixed to use actual simulationDatas structure from your codebase
//         if (m_context.simulationDatas) {
//             QJsonObject results;

//             // Convert your simulation results to JSON arrays
//             QJsonArray timeData, speedData, powerData, currentData;
//             QJsonArray phaseData, distanceData, accelerationData;

//             // Edited here: Using actual member variables from your SimulationDatas structure
//             for (int i = 0; i < m_context.simulationDatas->time.size(); ++i) {
//                 timeData.append(m_context.simulationDatas->time[i]);
//                 speedData.append(m_context.simulationDatas->trainSpeeds[i]);
//                 distanceData.append(m_context.simulationDatas->distance[i]);
//                 accelerationData.append(m_context.simulationDatas->accelerations[i]);
//                 phaseData.append(m_context.simulationDatas->phase[i]);
//             }

//             for (int i = 0; i < m_context.simulationDatas->powerCatenary.size(); ++i) {
//                 powerData.append(m_context.simulationDatas->powerCatenary[i]);
//                 currentData.append(m_context.simulationDatas->currentCatenary[i]);
//             }

//             results["time"] = timeData;
//             results["speed"] = speedData;
//             results["distance"] = distanceData;
//             results["acceleration"] = accelerationData;
//             results["phase"] = phaseData;
//             results["power"] = powerData;
//             results["current"] = currentData;

//             response["status"] = "success";
//             response["results"] = results;
//         } else {
//             response["status"] = "error";
//             response["message"] = "No simulation results available";
//         }
//     } catch (const std::exception &e) {
//         response["status"] = "error";
//         response["message"] = QString("Error retrieving results: %1").arg(e.what());
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// QHttpServerResponse ApiHandler::handleExportResults(const QJsonObject &data)
// {
//     QJsonObject response;

//     try {
//         QString format = data.value("format").toString("csv");

//         if (format == "csv") {
//             // Edited here: Use actual CSV output handler method
//             m_csvOutputHandler->printSimulationDatas();

//             response["status"] = "success";
//             response["message"] = "Results exported to CSV successfully";
//         } else {
//             response["status"] = "error";
//             response["message"] = "Unsupported export format";
//         }
//     } catch (const std::exception &e) {
//         response["status"] = "error";
//         response["message"] = QString("Error exporting results: %1").arg(e.what());
//     }

//     return QHttpServerResponse(QJsonDocument(response).toJson(),
//                                QHttpServerResponse::StatusCode::Ok);
// }

// // #include "api_handler.h"
// // #include <QJsonDocument>
// // #include <QJsonObject>
// // #include <QJsonArray>
// // #include <QDebug>
// // #include <QHttpServerResponse>
// // #include <QDateTime>

// // // Edited here: Fixed constructor to match actual data handler classes and removed non-existent ones
// // ApiHandler::ApiHandler(AppContext &context, QObject *parent)
// //     : QObject(parent), m_context(context)
// // {
// //     // Initialize handlers with context - using actual handler classes from your codebase
// //     m_trainDataHandler = new TrainDataHandler(&context, this);
// //     m_electricalDataHandler = new ElectricalDataHandler(&context, this);
// //     m_runningDataHandler = new RunningDataHandler(&context, this);
// //     m_trackDataHandler = new TrackDataHandler(&context, this);
// //     m_simulationHandler = new TrainSimulationHandler(context, this);
// //     m_csvOutputHandler = new CsvOutputHandler(*context.simulationDatas); // Edited here: Fixed to use simulationDatas not SimulationData
// // }

// // QHttpServerResponse ApiHandler::handleHealthCheck()
// // {
// //     QJsonObject response;
// //     response["status"] = "ok";
// //     response["service"] = "Train Simulation Backend";
// //     response["timestamp"] = QDateTime::currentDateTime().toString(Qt::ISODate);

// //     return QHttpServerResponse(QJsonDocument(response).toJson(),
// //                                QHttpServerResponse::StatusCode::Ok);
// // }

// // QHttpServerResponse ApiHandler::handleGetTrainParameters()
// // {
// //     QJsonObject response;

// //     // Edited here: Fixed to use actual trainData structure from your models
// //     if (m_context.trainData) {
// //         QJsonObject trainParams;
// //         trainParams["tractionMotors"] = m_context.trainData->n_tm;
// //         trainParams["axles"] = m_context.trainData->n_axle;
// //         trainParams["wheelDiameter"] = m_context.trainData->wheel;
// //         trainParams["gearRatio"] = m_context.trainData->gearRatio;
// //         trainParams["loadPerCar"] = m_context.trainData->load_per_car;
// //         trainParams["passengerWeight"] = m_context.trainData->passenger_weight;
// //         trainParams["aerodynamicCoeff"] = m_context.trainData->aerodynamic_coeff;
// //         trainParams["rollingResistanceCoeff"] = m_context.trainData->rolling_resistance_coeff;

// //         response["trainParameters"] = trainParams;
// //         response["status"] = "success";
// //     } else {
// //         response["status"] = "error";
// //         response["message"] = "Train data not initialized";
// //     }

// //     return QHttpServerResponse(QJsonDocument(response).toJson(),
// //                                QHttpServerResponse::StatusCode::Ok);
// // }

// // QHttpServerResponse ApiHandler::handleUpdateTrainParameters(const QJsonObject &data)
// // {
// //     QJsonObject response;

// //     try {
// //         if (data.contains("trainParameters")) {
// //             QJsonObject trainParams = data["trainParameters"].toObject();

// //             // Edited here: Update train data using direct access to context data
// //             if (trainParams.contains("tractionMotors")) {
// //                 m_context.trainData->n_tm = trainParams["tractionMotors"].toDouble();
// //             }
// //             if (trainParams.contains("axles")) {
// //                 m_context.trainData->n_axle = trainParams["axles"].toDouble();
// //             }
// //             if (trainParams.contains("wheelDiameter")) {
// //                 m_context.trainData->wheel = trainParams["wheelDiameter"].toDouble();
// //             }
// //             if (trainParams.contains("gearRatio")) {
// //                 m_context.trainData->gearRatio = trainParams["gearRatio"].toDouble();
// //             }
// //             if (trainParams.contains("loadPerCar")) {
// //                 m_context.trainData->load_per_car = trainParams["loadPerCar"].toDouble();
// //             }
// //             if (trainParams.contains("passengerWeight")) {
// //                 m_context.trainData->passenger_weight = trainParams["passengerWeight"].toDouble();
// //             }
// //             if (trainParams.contains("aerodynamicCoeff")) {
// //                 m_context.trainData->aerodynamic_coeff = trainParams["aerodynamicCoeff"].toDouble();
// //             }
// //             if (trainParams.contains("rollingResistanceCoeff")) {
// //                 m_context.trainData->rolling_resistance_coeff = trainParams["rollingResistanceCoeff"].toDouble();
// //             }

// //             // Edited here: Removed storeFormInputs call since it expects QMap<QString, InputWidget*>
// //             // The data is already stored directly above

// //             response["status"] = "success";
// //             response["message"] = "Train parameters updated successfully";
// //         } else {
// //             response["status"] = "error";
// //             response["message"] = "No train parameters provided";
// //         }
// //     } catch (const std::exception &e) {
// //         response["status"] = "error";
// //         response["message"] = QString("Error updating parameters: %1").arg(e.what());
// //     }

// //     return QHttpServerResponse(QJsonDocument(response).toJson(),
// //                                QHttpServerResponse::StatusCode::Ok);
// // }

// // QHttpServerResponse ApiHandler::handleGetElectricalParameters()
// // {
// //     QJsonObject response;

// //     // Edited here: Fixed to use actual powerData and efficiencyData structures from your models
// //     if (m_context.powerData && m_context.efficiencyData) {
// //         QJsonObject electricalParams;
// //         // Using actual variables from PowerData struct
// //         electricalParams["wheelPower"] = m_context.powerData->p_wheel;
// //         electricalParams["motorOutPower"] = m_context.powerData->p_motorOut;
// //         electricalParams["motorInPower"] = m_context.powerData->p_motorIn;
// //         electricalParams["vvvfInPower"] = m_context.powerData->p_vvvfIn;
// //         electricalParams["catenaryPower"] = m_context.powerData->p_catenary;
// //         electricalParams["apsPower"] = m_context.powerData->p_aps;
// //         electricalParams["staticApsPower"] = m_context.powerData->stat_p_aps;

// //         // Using actual variables from EfficiencyData struct
// //         electricalParams["staticGearEfficiency"] = m_context.efficiencyData->stat_eff_gear;
// //         electricalParams["staticMotorEfficiency"] = m_context.efficiencyData->stat_eff_motor;
// //         electricalParams["staticVvvfEfficiency"] = m_context.efficiencyData->stat_eff_vvvf;

// //         response["electricalParameters"] = electricalParams;
// //         response["status"] = "success";
// //     } else {
// //         response["status"] = "error";
// //         response["message"] = "Electrical data not initialized";
// //     }

// //     return QHttpServerResponse(QJsonDocument(response).toJson(),
// //                                QHttpServerResponse::StatusCode::Ok);
// // }

// // QHttpServerResponse ApiHandler::handleUpdateElectricalParameters(const QJsonObject &data)
// // {
// //     QJsonObject response;

// //     try {
// //         if (data.contains("electricalParameters")) {
// //             QJsonObject electricalParams = data["electricalParameters"].toObject();

// //             // Edited here: Update actual PowerData variables
// //             if (electricalParams.contains("wheelPower")) {
// //                 m_context.powerData->p_wheel = electricalParams["wheelPower"].toDouble();
// //             }
// //             if (electricalParams.contains("motorOutPower")) {
// //                 m_context.powerData->p_motorOut = electricalParams["motorOutPower"].toDouble();
// //             }
// //             if (electricalParams.contains("motorInPower")) {
// //                 m_context.powerData->p_motorIn = electricalParams["motorInPower"].toDouble();
// //             }
// //             if (electricalParams.contains("vvvfInPower")) {
// //                 m_context.powerData->p_vvvfIn = electricalParams["vvvfInPower"].toDouble();
// //             }
// //             if (electricalParams.contains("catenaryPower")) {
// //                 m_context.powerData->p_catenary = electricalParams["catenaryPower"].toDouble();
// //             }
// //             if (electricalParams.contains("apsPower")) {
// //                 m_context.powerData->p_aps = electricalParams["apsPower"].toDouble();
// //             }
// //             if (electricalParams.contains("staticApsPower")) {
// //                 m_context.powerData->stat_p_aps = electricalParams["staticApsPower"].toDouble();
// //             }

// //             // Edited here: Update actual EfficiencyData variables
// //             if (electricalParams.contains("staticGearEfficiency")) {
// //                 m_context.efficiencyData->stat_eff_gear = electricalParams["staticGearEfficiency"].toDouble();
// //             }
// //             if (electricalParams.contains("staticMotorEfficiency")) {
// //                 m_context.efficiencyData->stat_eff_motor = electricalParams["staticMotorEfficiency"].toDouble();
// //             }
// //             if (electricalParams.contains("staticVvvfEfficiency")) {
// //                 m_context.efficiencyData->stat_eff_vvvf = electricalParams["staticVvvfEfficiency"].toDouble();
// //             }

// //             response["status"] = "success";
// //             response["message"] = "Electrical parameters updated successfully";
// //         } else {
// //             response["status"] = "error";
// //             response["message"] = "No electrical parameters provided";
// //         }
// //     } catch (const std::exception &e) {
// //         response["status"] = "error";
// //         response["message"] = QString("Error updating electrical parameters: %1").arg(e.what());
// //     }

// //     return QHttpServerResponse(QJsonDocument(response).toJson(),
// //                                QHttpServerResponse::StatusCode::Ok);
// // }

// // QHttpServerResponse ApiHandler::handleGetRunningParameters()
// // {
// //     QJsonObject response;

// //     // Edited here: Fixed to use actual movingData and resistanceData structures
// //     if (m_context.movingData && m_context.resistanceData) {
// //         QJsonObject runningParams;
// //         runningParams["maxSpeed"] = m_context.movingData->v_limit;
// //         runningParams["initialSpeed"] = m_context.movingData->v;
// //         runningParams["acceleration"] = m_context.movingData->acc;
// //         runningParams["deceleration"] = m_context.movingData->decc;
// //         runningParams["startingResistance"] = m_context.resistanceData->startRes;
// //         runningParams["weakeningPoint1"] = m_context.movingData->v_p1;
// //         runningParams["weakeningPoint2"] = m_context.movingData->v_p2;
// //         runningParams["weakeningPoint3"] = m_context.movingData->v_b1;
// //         runningParams["weakeningPoint4"] = m_context.movingData->v_b2;
// //         runningParams["differenceCoastingSpeed"] = m_context.movingData->v_diffCoast;
// //         runningParams["emergencyBrakeDeceleration"] = m_context.movingData->decc_emergency;

// //         response["runningParameters"] = runningParams;
// //         response["status"] = "success";
// //     } else {
// //         response["status"] = "error";
// //         response["message"] = "Running data not initialized";
// //     }

// //     return QHttpServerResponse(QJsonDocument(response).toJson(),
// //                                QHttpServerResponse::StatusCode::Ok);
// // }

// // QHttpServerResponse ApiHandler::handleUpdateRunningParameters(const QJsonObject &data)
// // {
// //     QJsonObject response;

// //     try {
// //         if (data.contains("runningParameters")) {
// //             QJsonObject runningParams = data["runningParameters"].toObject();

// //             // Edited here: Update movingData and resistanceData directly
// //             if (runningParams.contains("maxSpeed")) {
// //                 m_context.movingData->v_limit = runningParams["maxSpeed"].toDouble();
// //             }
// //             if (runningParams.contains("initialSpeed")) {
// //                 m_context.movingData->v = runningParams["initialSpeed"].toDouble();
// //             }
// //             if (runningParams.contains("acceleration")) {
// //                 m_context.movingData->acc = runningParams["acceleration"].toDouble();
// //             }
// //             if (runningParams.contains("deceleration")) {
// //                 m_context.movingData->decc = runningParams["deceleration"].toDouble();
// //             }
// //             if (runningParams.contains("startingResistance")) {
// //                 m_context.resistanceData->startRes = runningParams["startingResistance"].toDouble();
// //             }
// //             if (runningParams.contains("weakeningPoint1")) {
// //                 m_context.movingData->v_p1 = runningParams["weakeningPoint1"].toDouble();
// //             }
// //             if (runningParams.contains("weakeningPoint2")) {
// //                 m_context.movingData->v_p2 = runningParams["weakeningPoint2"].toDouble();
// //             }
// //             if (runningParams.contains("weakeningPoint3")) {
// //                 m_context.movingData->v_b1 = runningParams["weakeningPoint3"].toDouble();
// //             }
// //             if (runningParams.contains("weakeningPoint4")) {
// //                 m_context.movingData->v_b2 = runningParams["weakeningPoint4"].toDouble();
// //             }
// //             if (runningParams.contains("differenceCoastingSpeed")) {
// //                 m_context.movingData->v_diffCoast = runningParams["differenceCoastingSpeed"].toDouble();
// //             }
// //             if (runningParams.contains("emergencyBrakeDeceleration")) {
// //                 m_context.movingData->decc_emergency = runningParams["emergencyBrakeDeceleration"].toDouble();
// //             }

// //             response["status"] = "success";
// //             response["message"] = "Running parameters updated successfully";
// //         } else {
// //             response["status"] = "error";
// //             response["message"] = "No running parameters provided";
// //         }
// //     } catch (const std::exception &e) {
// //         response["status"] = "error";
// //         response["message"] = QString("Error updating running parameters: %1").arg(e.what());
// //     }

// //     return QHttpServerResponse(QJsonDocument(response).toJson(),
// //                                QHttpServerResponse::StatusCode::Ok);
// // }

// // QHttpServerResponse ApiHandler::handleGetTrackParameters()
// // {
// //     QJsonObject response;

// //     // Edited here: Added track parameters handling using actual data structures
// //     if (m_context.stationData && m_context.resistanceData) {
// //         QJsonObject trackParams;
// //         trackParams["numberOfStations"] = m_context.stationData->n_station;
// //         trackParams["stationDistance"] = m_context.movingData->x_station;

// //         response["trackParameters"] = trackParams;
// //         response["status"] = "success";
// //     } else {
// //         response["status"] = "error";
// //         response["message"] = "Track data not initialized";
// //     }

// //     return QHttpServerResponse(QJsonDocument(response).toJson(),
// //                                QHttpServerResponse::StatusCode::Ok);
// // }

// // QHttpServerResponse ApiHandler::handleUpdateTrackParameters(const QJsonObject &data)
// // {
// //     QJsonObject response;

// //     try {
// //         if (data.contains("trackParameters")) {
// //             QJsonObject trackParams = data["trackParameters"].toObject();

// //             // Edited here: Update track data directly
// //             if (trackParams.contains("numberOfStations")) {
// //                 m_context.stationData->n_station = trackParams["numberOfStations"].toDouble();
// //             }
// //             if (trackParams.contains("stationDistance")) {
// //                 m_context.movingData->x_station = trackParams["stationDistance"].toDouble();
// //             }

// //             response["status"] = "success";
// //             response["message"] = "Track parameters updated successfully";
// //         } else {
// //             response["status"] = "error";
// //             response["message"] = "No track parameters provided";
// //         }
// //     } catch (const std::exception &e) {
// //         response["status"] = "error";
// //         response["message"] = QString("Error updating track parameters: %1").arg(e.what());
// //     }

// //     return QHttpServerResponse(QJsonDocument(response).toJson(),
// //                                QHttpServerResponse::StatusCode::Ok);
// // }

// // QHttpServerResponse ApiHandler::handleStartSimulation(const QJsonObject &data)
// // {
// //     QJsonObject response;

// //     try {
// //         QString simulationType = data.value("type").toString("dynamic");

// //         if (simulationType == "dynamic") {
// //             // Edited here: Use correct method name from TrainSimulationHandler
// //             m_simulationHandler->simulateDynamicTrainMovement();

// //             response["status"] = "success";
// //             response["message"] = "Dynamic simulation started";
// //             response["type"] = "dynamic";
// //         } else {
// //             response["status"] = "error";
// //             response["message"] = "Unknown simulation type";
// //         }
// //     } catch (const std::exception &e) {
// //         response["status"] = "error";
// //         response["message"] = QString("Error starting simulation: %1").arg(e.what());
// //     }

// //     return QHttpServerResponse(QJsonDocument(response).toJson(),
// //                                QHttpServerResponse::StatusCode::Ok);
// // }

// // QHttpServerResponse ApiHandler::handleGetSimulationStatus()
// // {
// //     QJsonObject response;

// //     // Edited here: Simple status response - you may want to add actual status tracking
// //     response["status"] = "success";
// //     response["simulationRunning"] = false;
// //     response["progress"] = 100.0;
// //     response["message"] = "Simulation completed";

// //     return QHttpServerResponse(QJsonDocument(response).toJson(),
// //                                QHttpServerResponse::StatusCode::Ok);
// // }

// // QHttpServerResponse ApiHandler::handleGetSimulationResults()
// // {
// //     QJsonObject response;

// //     try {
// //         // Edited here: Fixed to use actual simulationDatas structure from your codebase
// //         if (m_context.simulationDatas) {
// //             QJsonObject results;

// //             // Convert your simulation results to JSON arrays
// //             QJsonArray timeData, speedData, powerData, currentData;
// //             QJsonArray phaseData, distanceData, accelerationData;

// //             // Edited here: Using actual member variables from your SimulationDatas structure
// //             for (int i = 0; i < m_context.simulationDatas->time.size(); ++i) {
// //                 timeData.append(m_context.simulationDatas->time[i]);
// //                 speedData.append(m_context.simulationDatas->trainSpeeds[i]);
// //                 distanceData.append(m_context.simulationDatas->distance[i]);
// //                 accelerationData.append(m_context.simulationDatas->accelerations[i]);
// //                 phaseData.append(m_context.simulationDatas->phase[i]);
// //             }

// //             for (int i = 0; i < m_context.simulationDatas->powerCatenary.size(); ++i) {
// //                 powerData.append(m_context.simulationDatas->powerCatenary[i]);
// //                 currentData.append(m_context.simulationDatas->currentCatenary[i]);
// //             }

// //             results["time"] = timeData;
// //             results["speed"] = speedData;
// //             results["distance"] = distanceData;
// //             results["acceleration"] = accelerationData;
// //             results["phase"] = phaseData;
// //             results["power"] = powerData;
// //             results["current"] = currentData;

// //             response["status"] = "success";
// //             response["results"] = results;
// //         } else {
// //             response["status"] = "error";
// //             response["message"] = "No simulation results available";
// //         }
// //     } catch (const std::exception &e) {
// //         response["status"] = "error";
// //         response["message"] = QString("Error retrieving results: %1").arg(e.what());
// //     }

// //     return QHttpServerResponse(QJsonDocument(response).toJson(),
// //                                QHttpServerResponse::StatusCode::Ok);
// // }

// // QHttpServerResponse ApiHandler::handleExportResults(const QJsonObject &data)
// // {
// //     QJsonObject response;

// //     try {
// //         QString format = data.value("format").toString("csv");

// //         if (format == "csv") {
// //             // Edited here: Use actual CSV output handler method
// //             m_csvOutputHandler->printSimulationDatas();

// //             response["status"] = "success";
// //             response["message"] = "Results exported to CSV successfully";
// //         } else {
// //             response["status"] = "error";
// //             response["message"] = "Unsupported export format";
// //         }
// //     } catch (const std::exception &e) {
// //         response["status"] = "error";
// //         response["message"] = QString("Error exporting results: %1").arg(e.what());
// //     }

// //     return QHttpServerResponse(QJsonDocument(response).toJson(),
// //                                QHttpServerResponse::StatusCode::Ok);
// // }
