#ifndef UTILITY_HANDLER_H
#define UTILITY_HANDLER_H

#include "../models/train_data.h"
#include <QObject>

class UtilityHandler : public QObject {
public:
  explicit UtilityHandler();
  void addSimulationDatas(int i, double time, QString phase);
  void resetSimulation();
  void clearSimulationDatas();
};

#endif // UTILITY_HANDLER_H
