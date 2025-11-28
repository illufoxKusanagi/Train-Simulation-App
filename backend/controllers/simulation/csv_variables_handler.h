#ifndef CSV_VARIABLES_HANDLER_H
#define CSV_VARIABLES_HANDLER_H

#include "core/appcontext.h"
#include <QObject>
#include <QSet>

class CsvVariableHandler : public QObject {
  Q_OBJECT
public:
  explicit CsvVariableHandler(AppContext &context,
                              QStringList *simulationWarnings = nullptr);

  int setSlopeIndex(int slopeIndex, double distanceTravelled);
  int setRadiusIndex(int radiusIndex, double distanceTravelled);
  int setMaxSpeedIndex(int maxSpeedIndex, double distanceTravelled);
  int setEffGearIndex(int effGearIndex, double speed);
  int setEffVvvfIndex(int effVvvfIndex, double speed);
  int setEffMotorIndex(int effMotorIndex, double speed);
  int setLineVoltageIndex(int lineVoltageIndex, double speed);
  int setMotorVoltageIndex(int motorVoltageIndex, double speed);
  double setSlopeData(int slopeIndex, double distanceTravelled);
  double setRadiusData(int radiusIndex, double distanceTravelled);
  double setMaxSpeedData(int maxSpeedIndex, double distanceTravelled);
  double setEffGearData(int effGearIndex, double speed);
  double setEffVvvfData(int effVvvfIndex, double speed);
  double setEffMotorData(int effMotorIndex, double speed);
  double setLineVoltageData(int lineVoltageIndex, double speed);
  double setMotorVoltageData(int motorVoltageIndex, double speed);
  double setDwellTimeData(int dwellTimeIndex);

private:
  StationData *stationData;
  EfficiencyData *efficiencyData;
  EnergyData *energyData;
  QStringList *m_simulationWarnings;

  // Generic helper to update index based on current position/value
  int updateIndex(const std::vector<double> &thresholds, int currentIndex,
                  double currentValue);

  // Generic helper to get interpolated/step value
  // T can be double or float (usually double in this app)
  template <typename T>
  T getValueAt(const std::vector<double> &thresholds,
               const std::vector<T> &values, int &currentIndex,
               double currentValue, T defaultValue, const QString &dataName);
};

#endif // CSV_VARIABLES_HANDLER_H
