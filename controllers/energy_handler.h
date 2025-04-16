#ifndef ENERGY_HANDLER_H
#define ENERGY_HANDLER_H

#include "core/appcontext.h"
#include "models/load_data.h"
#include "models/power_data.h"
#include "models/train_data.h"
#include <QObject>

class EnergyHandler : public QObject {
public:
  explicit EnergyHandler(AppContext &context);
  double calculateEnergyConsumption(int i);
  double calculateEnergyOfPowering(int i);
  double calculateEnergyRegeneration(int i);
  double calculateEnergyOfAps(int i);

private:
  SimulationDatas *simulationDatas;
  PowerData *powerData;
  LoadData *loadData;
};

#endif // ENERGY_HANDLER_H
