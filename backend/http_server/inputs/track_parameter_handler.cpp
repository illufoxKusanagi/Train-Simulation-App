#include "track_parameter_handler.h"

TrackParameterHandler::TrackParameterHandler(AppContext &context, QObject *parent)
    : QObject{parent}, m_context(context)
{
}

QHttpServerResponse TrackParameterHandler::handleGetTrackParameters()
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

QHttpServerResponse TrackParameterHandler::handleUpdateTrackParameters(const QJsonObject &data)
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