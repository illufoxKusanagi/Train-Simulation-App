#ifndef TRACTIVE_EFFORT_HANDLER_H
#define TRACTIVE_EFFORT_HANDLER_H

#include "models/train_data.h"
#include "widgets/message_box_widget.h"
#include <QObject>

class TractiveEffortHandler : public QObject {
  Q_OBJECT
public:
  explicit TractiveEffortHandler(MassData &massData,
                                 ResistanceData &resistanceData,
                                 TrainData &trainData, MovingData &movingData,
                                 TrainMotorData &trainMotorData);
  double calculateStartForce(float acc);
  double calculateTotalForce(float v);
  double calculateTotalBrakeForce();
  double calculateTractionForce();
  void calculatePoweringForce(float acc, float v);
  void calculateBrakingForce();
  double calculateAdhesion();

private:
  MassData *massData;
  ResistanceData *resistanceData;
  TrainData *trainData;
  MovingData *movingData;
  TrainMotorData *trainMotorData;
  ConstantData *constantData;
  LoadData *loadData;
};

#endif // TRACTIVE_EFFORT_HANDLER_H
