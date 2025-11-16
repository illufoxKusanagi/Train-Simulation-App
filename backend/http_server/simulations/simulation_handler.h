#ifndef SIMULATION_HANDLER_H
#define SIMULATION_HANDLER_H

#include <QObject>
#include <qhttpserverresponse.h>
#include <qjsonobject.h>
#include <qjsonarray.h>
#include <core/appcontext.h>
#include "controllers/simulation/train_simulation_handler.h"

class SimulationHandler : public QObject
{
    Q_OBJECT
public:
    explicit SimulationHandler(AppContext &context, QObject *parent = nullptr);
    QHttpServerResponse handleStartSimulation(const QJsonObject &data);
    QHttpServerResponse handleGetSimulationStatus();
    QHttpServerResponse handleGetSimulationResults();

private:
    AppContext &m_context;
    TrainSimulationHandler *m_simulationHandler;
};

#endif // SIMULATION_HANDLER_H
