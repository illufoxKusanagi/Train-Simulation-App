#ifndef TRAIN_PARAMETER_HANDLER_H
#define TRAIN_PARAMETER_HANDLER_H

#include <QObject>
#include <core/appcontext.h>
#include <qhttpserverresponse.h>
#include <qjsonobject.h>

// Forward declaration
class MassHandler;

class TrainParameterHandler : public QObject {
  Q_OBJECT
public:
  explicit TrainParameterHandler(AppContext &context,
                                 QObject *parent = nullptr);
  ~TrainParameterHandler();
  QHttpServerResponse handleGetTrainParameters();
  QHttpServerResponse handleUpdateTrainParameters(const QJsonObject &data);
  QHttpServerResponse handleGetCarNumberParameters();
  QHttpServerResponse handleUpdateCarNumberParameters(const QJsonObject &data);
  QHttpServerResponse handleGetPassengerParameters();
  QHttpServerResponse handleUpdatePassengerParameters(const QJsonObject &data);
  QHttpServerResponse handleGetMassParameters();
  QHttpServerResponse handleUpdateMassParameters(const QJsonObject &data);
  QHttpServerResponse handleCalculateMass(const QJsonObject &data);

private:
  AppContext &m_context;
  MassHandler *m_massHandler;

  void recalculateMasses();
};

#endif // TRAIN_PARAMETER_HANDLER_H
