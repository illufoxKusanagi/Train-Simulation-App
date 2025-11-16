#ifndef TRAIN_PARAMETER_HANDLER_H
#define TRAIN_PARAMETER_HANDLER_H

#include <QObject>
#include <qhttpserverresponse.h>
#include <qjsonobject.h>
#include <core/appcontext.h>

class TrainParameterHandler : public QObject
{
    Q_OBJECT
public:
    explicit TrainParameterHandler(AppContext &context, QObject *parent = nullptr);
    QHttpServerResponse handleGetTrainParameters();
    QHttpServerResponse handleUpdateTrainParameters(const QJsonObject &data);
    QHttpServerResponse handleGetCarNumberParameters();
    QHttpServerResponse handleUpdateCarNumberParameters(const QJsonObject &data);
    QHttpServerResponse handleGetPassengerParameters();
    QHttpServerResponse handleUpdatePassengerParameters(const QJsonObject &data);
    QHttpServerResponse handleGetMassParameters();
    QHttpServerResponse handleUpdateMassParameters(const QJsonObject &data);

private:
    AppContext &m_context;
};

#endif // TRAIN_PARAMETER_HANDLER_H
