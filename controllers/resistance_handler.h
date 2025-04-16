#ifndef RESISTANCE_HANDLER_H
#define RESISTANCE_HANDLER_H

#include "models/train_data.h"
#include <QObject>

class ResistanceHandler : public QObject {
  Q_OBJECT
public:
  explicit ResistanceHandler(ResistanceData &resistanceData,
                             ConstantData &constantData);
  double calculateResTrain(float m, float startRes);
  double calculateResSlope(float m, float slope);
  double calculateResRadius(float m, float radius);

private:
  ResistanceData *resistanceData;
  ConstantData *constantData;
};

#endif // RESISTANCE_HANDLER_H
