#ifndef UTILITY_HANDLER_H
#define UTILITY_HANDLER_H

#include "core/appcontext.h"
#include "models/simulation_data.h"
#include "models/train_data.h"
#include <QObject>

class UtilityHandler : public QObject {
public slots:
  void resetSimulation();

public:
  explicit UtilityHandler(AppContext &context,
                          SimulationDatas &simulationDatas);
  void addSimulationDatas(int i, double time, QString phase);
  void clearSimulationDatas();

private:
  AppContext *context;
  SimulationDatas *simulationDatas;
};

#endif // UTILITY_HANDLER_H
