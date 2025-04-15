#ifndef TRACTION_MOTOR_HANDLER_H
#define TRACTION_MOTOR_HANDLER_H

#include "../models/train_data.h"
#include <QObject>

class TractionMotorHandler : public QObject {
  Q_OBJECT
public:
  explicit TractionMotorHandler(TrainData &trainData,
                                ResistanceData &resistanceData,
                                MovingData &movingData,
                                TrainMotorData &trainMotorData);
  double calculateTractionForce();
  double calculateTorque();
  double calculateRpm();

private:
  TrainData *trainData;
  ResistanceData *resistanceData;
  MovingData *movingData;
  TrainMotorData *trainMotorData;
};

#endif // TRACTION_MOTOR_HANDLER_H
