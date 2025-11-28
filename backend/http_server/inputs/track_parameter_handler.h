#ifndef TRACK_PARAMETER_HANDLER_H
#define TRACK_PARAMETER_HANDLER_H

#include <QObject>
#include <qhttpserverresponse.h>
#include <qjsonobject.h>
#include <core/appcontext.h>

class TrackParameterHandler : public QObject
{
    Q_OBJECT
public:
    explicit TrackParameterHandler(AppContext &context, QObject *parent = nullptr);
    QHttpServerResponse handleGetTrackParameters();
    QHttpServerResponse handleUpdateTrackParameters(const QJsonObject &data);

private:
    AppContext &m_context;
};

#endif // TRACK_PARAMETER_HANDLER_H
