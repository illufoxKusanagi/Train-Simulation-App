
#include "csv_variables_handler.h"

CsvVariableHandler::CsvVariableHandler(AppContext &context,
                                       QStringList *simulationWarnings)
    : stationData(context.stationData.data()),
      efficiencyData(context.efficiencyData.data()),
      energyData(context.energyData.data()),
      m_simulationWarnings(simulationWarnings) {}

int CsvVariableHandler::updateIndex(const std::vector<double> &thresholds,
                                    int currentIndex, double currentValue) {
  if (!thresholds.empty()) {
    // Check if we need to advance the index
    if (currentIndex < thresholds.size() &&
        currentValue >= thresholds[currentIndex]) {
      return currentIndex + 1;
    }
    // Check if we need to go back (only applicable for speed-based lookups
    // where speed can decrease) This condition is typically for
    // efficiency/voltage data where the index might need to decrease if speed
    // drops. For distance-based data (slope, radius, max speed), currentValue
    // (distance) is always increasing, so this branch won't be taken unless
    // there's an external reset or error.
    else if (currentIndex > 0 && currentIndex <= thresholds.size() &&
             currentValue < thresholds[currentIndex - 1]) {
      return currentIndex - 1;
    }
    return currentIndex; // No change needed
  }
  // If thresholds are empty, always return 0 as there's no data to index.
  // The original code for empty data returned 0.
  return 0;
}

template <typename T>
T CsvVariableHandler::getValueAt(const std::vector<double> &thresholds,
                                 const std::vector<T> &values,
                                 int &currentIndex, double currentValue,
                                 T defaultValue, const QString &dataName) {
  if (!values.empty() && currentIndex < values.size()) {
    // Update index first
    if (currentValue >= thresholds[currentIndex]) {
      currentIndex++;
    } else if (currentIndex > 0 && currentValue < thresholds[currentIndex]) {
      return values[currentIndex - 1];
    }

    // Check bounds again after increment
    if (currentIndex >= values.size())
      return values.back();
    return values[currentIndex];
  } else if (values.empty()) {
    m_simulationWarnings->append(dataName + " csv data is empty.");
  } else {
    m_simulationWarnings->append("Attempted to access " + dataName +
                                 " data beyond range.");
  }
  return defaultValue;
}

int CsvVariableHandler::setSlopeIndex(int slopeIndex,
                                      double distanceTravelled) {
  return updateIndex(stationData->x_slopeEnd, slopeIndex, distanceTravelled);
}

int CsvVariableHandler::setRadiusIndex(int radiusIndex,
                                       double distanceTravelled) {
  return updateIndex(stationData->x_radiusEnd, radiusIndex, distanceTravelled);
}

int CsvVariableHandler::setMaxSpeedIndex(int maxSpeedIndex,
                                         double distanceTravelled) {
  return updateIndex(stationData->x_v_limitEnd, maxSpeedIndex,
                     distanceTravelled);
}

int CsvVariableHandler::setEffGearIndex(int effGearIndex, double speed) {
  return updateIndex(efficiencyData->v_eff_gear, effGearIndex, speed);
}

int CsvVariableHandler::setEffVvvfIndex(int effVvvfIndex, double speed) {
  return updateIndex(efficiencyData->v_eff_vvvf, effVvvfIndex, speed);
}

int CsvVariableHandler::setEffMotorIndex(int effMotorIndex, double speed) {
  return updateIndex(efficiencyData->v_eff_motor, effMotorIndex, speed);
}

int CsvVariableHandler::setLineVoltageIndex(int lineVoltageIndex,
                                            double speed) {
  return updateIndex(energyData->v_vol_line, lineVoltageIndex, speed);
}

int CsvVariableHandler::setMotorVoltageIndex(int motorVoltageIndex,
                                             double speed) {
  return updateIndex(energyData->v_vol_motor, motorVoltageIndex, speed);
}

double CsvVariableHandler::setSlopeData(int slopeIndex,
                                        double distanceTravelled) {
  return getValueAt(stationData->x_slopeEnd, stationData->slope, slopeIndex,
                    distanceTravelled, stationData->stat_slope, "Slope");
}

double CsvVariableHandler::setRadiusData(int radiusIndex,
                                         double distanceTravelled) {
  return getValueAt(stationData->x_radiusEnd, stationData->radius, radiusIndex,
                    distanceTravelled, stationData->stat_radius, "Radius");
}

double CsvVariableHandler::setMaxSpeedData(int maxSpeedIndex,
                                           double distanceTravelled) {
  return getValueAt(stationData->x_v_limitEnd, stationData->v_limit,
                    maxSpeedIndex, distanceTravelled, stationData->stat_v_limit,
                    "Speed limit");
}

double CsvVariableHandler::setEffVvvfData(int effVvvfIndex, double speed) {
  return getValueAt(efficiencyData->v_eff_vvvf, efficiencyData->eff_vvvf,
                    effVvvfIndex, speed, efficiencyData->stat_eff_vvvf,
                    "VVVF efficiency");
}

double CsvVariableHandler::setEffGearData(int effGearIndex, double speed) {
  return getValueAt(efficiencyData->v_eff_gear, efficiencyData->eff_gear,
                    effGearIndex, speed, efficiencyData->stat_eff_gear,
                    "Gear efficiency");
}

double CsvVariableHandler::setEffMotorData(int effMotorIndex, double speed) {
  return getValueAt(efficiencyData->v_eff_motor, efficiencyData->eff_motor,
                    effMotorIndex, speed, efficiencyData->stat_eff_motor,
                    "Motor efficiency");
}

double CsvVariableHandler::setLineVoltageData(int lineVoltageIndex,
                                              double speed) {
  return getValueAt(energyData->v_vol_line, energyData->vol_line,
                    lineVoltageIndex, speed, energyData->stat_vol_line,
                    "Line voltage");
}

double CsvVariableHandler::setMotorVoltageData(int motorVoltageIndex,
                                               double speed) {
  return getValueAt(energyData->v_vol_motor, energyData->vol_motor,
                    motorVoltageIndex, speed, energyData->stat_vol_motor,
                    "Motor voltage");
}

double CsvVariableHandler::setDwellTimeData(int dwellTimeIndex) {
  if (!stationData->dwellTime.empty() &&
      dwellTimeIndex < stationData->dwellTime.size()) {
    return stationData->dwellTime[dwellTimeIndex];
  } else if (stationData->dwellTime.empty()) {
    m_simulationWarnings->append("Dwell time csv data is empty.");
  } else {
    m_simulationWarnings->append(
        "Attempted to access dwell time data beyond range.");
  }
  return stationData->stat_dwellTime;
}