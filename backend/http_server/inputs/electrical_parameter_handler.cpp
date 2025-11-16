#include "electrical_parameter_handler.h"

ElectricalParameterHandler::ElectricalParameterHandler(AppContext &context, QObject *parent)
    : QObject{parent}, m_context(context)
{
}

QHttpServerResponse ElectricalParameterHandler::handleGetElectricalParameters()
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

QHttpServerResponse ElectricalParameterHandler::handleUpdateElectricalParameters(const QJsonObject &data)
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
