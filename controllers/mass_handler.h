#ifndef MASS_HANDLER_H
#define MASS_HANDLER_H

#include "models/load_data.h"
#include "models/mass_data.h"
#include "models/train_data.h"

#include <QObject>

class MassHandler : public QObject {
  Q_OBJECT
public:
  explicit MassHandler(LoadData &loadData, MassData &massData,
                       TrainData &trainData);
  double countMassEmptyCar();
  double countMassWithLoad();
  double countMassLoadInput();
  double countInertialMass();
  double countInertialMassInput();

private:
  LoadData *loadData;
  MassData *massData;
  TrainData *trainData;
};

#endif // MASS_HANDLER_H
