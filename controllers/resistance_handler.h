#ifndef RESISTANCE_HANDLER_H
#define RESISTANCE_HANDLER_H

#include "core/appcontext.h"
#include <QObject>

class ResistanceHandler : public QObject {
  Q_OBJECT
public:
  explicit ResistanceHandler(AppContext &context);
  double calculateResTrain(float m, float startRes);
  double calculateResSlope(float m, float slope);
  double calculateResRadius(float m, float radius);

private:
  ResistanceData *resistanceData;
  ConstantData *constantData;
};

#endif // RESISTANCE_HANDLER_H
