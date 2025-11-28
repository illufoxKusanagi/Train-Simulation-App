#ifndef SIMULATION_HANDLER_H
#define SIMULATION_HANDLER_H

#include "controllers/simulation/train_simulation_handler.h"
#include <QObject>
#include <core/appcontext.h>
#include <qhttpserverresponse.h>
#include <qjsonarray.h>
#include <qjsonobject.h>

class SimulationHandler : public QObject {
  Q_OBJECT
public:
  explicit SimulationHandler(AppContext &context, QObject *parent = nullptr);
  QHttpServerResponse handleStartSimulation(const QJsonObject &data);
  QHttpServerResponse handleGetSimulationStatus();
  QHttpServerResponse handleGetSimulationResults();

private:
  QJsonObject getStaticResults();
  QJsonObject getDynamicResults();

  AppContext &m_context;
  TrainSimulationHandler *m_trainSimulation;
  QString m_currentSimulationType;
};

#endif // SIMULATION_HANDLER_H
