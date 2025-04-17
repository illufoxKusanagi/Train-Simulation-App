#include "output_handler.h"
using namespace std;

OutputHandler::OutputHandler(SimulationDatas &simulationDatas)
    : simulationDatas(&simulationDatas) {}

void OutputHandler::printSimulationDatas() {
  int maxSize = getAllDataNumber();
  if (simulationDatas->energyRegenerations.size() < maxSize) {
    simulationDatas->energyRegenerations.resize(maxSize);
  }
  QString filepath = QFileDialog::getSaveFileName(
      nullptr, "Save File", QDir::homePath(), "CSV File (*.csv)");
  qDebug() << filepath;
  if (filepath.isEmpty()) {
    MessageBoxWidget messagebox("Alert", "The process canceled by user",
                                MessageBoxWidget::Warning);
    return;
  }
  if (!filepath.endsWith(".csv", Qt::CaseInsensitive)) {
    filepath += ".csv";
  }
  ofstream outFile(filepath.toStdString(), ios::out);
  outFile << "Phase,Iteration,Time (s),Total "
             "time (s),Distance (m),TotalDistance (m),Mileage (m),Speed "
             "(km/h),Speed (m/s),Acceleration (km/h/s),Acceleration (m/s2),F "
             "Motor,F Res, F Total, F Motor / TM, F Res / TM, Torque, RPM,P "
             "Wheel,P_motor Out,P_motor In,P_vvvf, P_catenary,Catenary "
             "current,VVVF current,Energy Consumption,Energy of "
             "Powering,Energy Regen,Energy of APS,Energy Catenary\n";
  for (int i = 0; i < maxSize; i++) {
    outFile << simulationDatas->phase[i].toStdString() << "," << i + 1 << ","
            << simulationDatas->time[i] << "," << simulationDatas->timeTotal[i]
            << "," << simulationDatas->distance[i] << ","
            << simulationDatas->distanceTotal[i] << ","
            << (simulationDatas->mileages.isEmpty()
                    ? 0
                    : simulationDatas->mileages[i])
            << "," << simulationDatas->trainSpeeds[i] << ","
            << simulationDatas->trainSpeedsSi[i] << ","
            << simulationDatas->accelerations[i] << ","
            << simulationDatas->accelerationsSi[i] << ","
            << simulationDatas->motorForce[i] << ","
            << simulationDatas->motorResistance[i] << ","
            << simulationDatas->totalResistance[i] << ","
            << simulationDatas->tractionForcePerMotor[i] << ","
            << simulationDatas->resistancePerMotor[i] << ","
            << simulationDatas->torque[i] << "," << simulationDatas->rpm[i]
            << "," << simulationDatas->powerWheel[i] << ","
            << simulationDatas->powerMotorOut[i] << ","
            << simulationDatas->powerMotorIn[i] << ","
            << simulationDatas->vvvfPowers[i] << ","
            << simulationDatas->catenaryPowers[i] << ","
            << simulationDatas->catenaryCurrents[i] << ","
            << simulationDatas->vvvfCurrents[i] << ","
            << simulationDatas->energyConsumptions[i] << ","
            << simulationDatas->energyPowerings[i] << ","
            << simulationDatas->energyRegenerations[i] << ","
            << simulationDatas->energyAps[i] << ","
            << simulationDatas->energyCatenaries[i] << "\n";
  }
  outFile.close();
  MessageBoxWidget messagebox("Information",
                              "Simulation data saved at " + filepath,
                              MessageBoxWidget::Information);
}

bool OutputHandler::saveTrainSpeedData() {
  QString filepath = QFileDialog::getSaveFileName(
      nullptr, "Save File", QDir::homePath(), "CSV File (*.csv)");
  if (filepath.isEmpty()) {
    MessageBoxWidget messagebox("Alert", "The process canceled by user",
                                MessageBoxWidget::Warning);
    return false;
  }
  ofstream outFile(filepath.toStdString(), ios::out);
  outFile << "Speed (km/h),Time (s)\n ";
  int maxSize = getTrainSpeedDataNumber();
  for (int i = 0; i < maxSize; i++) {
    outFile << simulationDatas->trainSpeeds[i] << ","
            << simulationDatas->timeTotal[i] << "\n";
  }
  outFile.close();
  return true;
}

bool OutputHandler::saveTractionEffortData() {
  QString filepath = QFileDialog::getSaveFileName(
      nullptr, "Save File", QDir::homePath(), "CSV File (*.csv)");
  if (filepath.isEmpty()) {
    MessageBoxWidget messagebox("Alert", "The process canceled by user",
                                MessageBoxWidget::Warning);
    return false;
  }
  ofstream outFile(filepath.toStdString(), ios::out);
  outFile << "F motor (kN),Speed(km/h),Time(s)\n ";
  int maxSize = getTractionEffortDataNumber();
  for (int i = 0; i < maxSize; i++) {
    outFile << simulationDatas->tractionEfforts[i] << ","
            << simulationDatas->trainSpeeds[i] << ","
            << simulationDatas->timeTotal[i] << "\n";
  }
  outFile.close();
  return true;
}

bool OutputHandler::saveTrainPowerData() {
  QString filepath = QFileDialog::getSaveFileName(
      nullptr, "Save File", QDir::homePath(), "CSV File (*.csv)");
  if (filepath.isEmpty()) {
    MessageBoxWidget messagebox("Alert", "The process canceled by user",
                                MessageBoxWidget::Warning);
    return false;
  }
  ofstream outFile(filepath.toStdString(), ios::out);
  outFile << "P_vvvf(kW),P_catenary(kW),Catenary "
             "current(A),VVVFcurrent(A),Speed(km/h),time(s)\n ";
  int maxSize = getTrainPowerDataNumber();
  for (int i = 0; i < maxSize; i++) {
    outFile << simulationDatas->vvvfPowers[i] << ","
            << simulationDatas->catenaryPowers[i] << ","
            << simulationDatas->vvvfCurrents[i] << ","
            << simulationDatas->catenaryCurrents[i] << ","
            << simulationDatas->trainSpeeds[i] << ","
            << simulationDatas->timeTotal[i] << "\n";
  }
  outFile.close();
  return true;
}

bool OutputHandler::saveTrainTrackData() {
  QString filepath = QFileDialog::getSaveFileName(
      nullptr, "Save File", QDir::homePath(), "CSV File (*.csv)");
  if (filepath.isEmpty()) {
    MessageBoxWidget messagebox("Alert", "The process canceled by user",
                                MessageBoxWidget::Warning);
    return false;
  }
  ofstream outFile(filepath.toStdString(), ios::out);
  outFile << "Simulation Time(s),Total "
             "Time(s),Distance(m),TotalDistance(m),Speed(km/h)\n ";
  int maxSize = getTrainTrackDataNumber();
  for (int i = 0; i < maxSize; i++) {
    outFile << simulationDatas->time[i] << "," << simulationDatas->timeTotal[i]
            << "," << simulationDatas->distance[i] << ","
            << simulationDatas->distanceTotal[i] << ","
            << simulationDatas->trainSpeeds[i] << "\n";
  }
  outFile.close();
  return true;
}

bool OutputHandler::saveEnergyConsumptionData() {
  QString filepath = QFileDialog::getSaveFileName(
      nullptr, "Save File", QDir::homePath(), "CSV File (*.csv)");
  if (filepath.isEmpty()) {
    MessageBoxWidget messagebox("Alert", "The process canceled by user",
                                MessageBoxWidget::Warning);
    return false;
  }
  ofstream outFile(filepath.toStdString(), ios::out);
  outFile << "Energy Consumption (kW),Energy Powering(kW),Energy Regen "
             "(kW),Energy of APS (kW)\n ";
  int maxSize = std::min({
      simulationDatas->energyConsumptions.size(),
      simulationDatas->energyPowerings.size(),
      simulationDatas->energyAps.size(),
  });
  if (simulationDatas->energyRegenerations.size() < maxSize) {
    simulationDatas->energyRegenerations.resize(maxSize);
  }
  for (int i = 0; i < maxSize; i++) {
    outFile << simulationDatas->energyConsumptions[i] << ","
            << simulationDatas->energyPowerings[i] << ","
            << (simulationDatas->energyRegenerations.isEmpty()
                    ? 0
                    : simulationDatas->energyRegenerations[i])
            << "," << simulationDatas->energyAps[i] << "\n";
  }
  outFile.close();
  return true;
}

double OutputHandler::findMaxSpeed() {
  if (simulationDatas->trainSpeeds.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->trainSpeeds.begin(),
                           simulationDatas->trainSpeeds.end());
}

double OutputHandler::findMaxVvvfPower() {
  if (simulationDatas->vvvfPowers.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->vvvfPowers.begin(),
                           simulationDatas->vvvfPowers.end());
}

double OutputHandler::findMaxCatenaryPower() {
  if (simulationDatas->catenaryPowers.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->catenaryPowers.begin(),
                           simulationDatas->catenaryPowers.end());
}

double OutputHandler::findMaxVvvfCurrent() {
  if (simulationDatas->vvvfCurrents.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->vvvfCurrents.begin(),
                           simulationDatas->vvvfCurrents.end());
}

double OutputHandler::findMaxCatenaryCurrent() {
  if (simulationDatas->catenaryCurrents.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->catenaryCurrents.begin(),
                           simulationDatas->catenaryCurrents.end());
}

double OutputHandler::findMaxTractionEffort() {
  if (simulationDatas->tractionEfforts.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->tractionEfforts.begin(),
                           simulationDatas->tractionEfforts.end());
}

double OutputHandler::findDistanceTravelled() {
  if (simulationDatas->distanceTotal.isEmpty())
    return 0.0;
  return simulationDatas->distanceTotal.last();
}

double OutputHandler::findMaxEnergyConsumption() {
  if (simulationDatas->energyConsumptions.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->energyConsumptions.begin(),
                           simulationDatas->energyConsumptions.end());
}

double OutputHandler::findMaxEnergyRegen() {
  if (simulationDatas->energyRegenerations.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->energyRegenerations.begin(),
                           simulationDatas->energyRegenerations.end());
}

double OutputHandler::findMaxEnergyPowering() {
  if (simulationDatas->energyPowerings.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->energyPowerings.begin(),
                           simulationDatas->energyPowerings.end());
}

double OutputHandler::findMaxEnergyAps() {
  if (simulationDatas->energyAps.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->energyAps.begin(),
                           simulationDatas->energyAps.end());
}

double OutputHandler::findMaxCurrTime() {
  double maxCurrTime = 0.0;
  double maxVvvfCurrent = findMaxVvvfCurrent();
  const double epsilon = 0.001; // Precision on decimal double value
  for (int i = 0; i < simulationDatas->vvvfCurrents.size(); i++) {
    if (std::abs(simulationDatas->vvvfCurrents[i] - maxVvvfCurrent) < epsilon) {
      maxCurrTime += simulationDatas->time[i];
    }
  }
  return maxCurrTime;
}

double OutputHandler::findMaxPowTime() {
  double maxPowTime = 0.0;
  double maxVvvfPower = findMaxVvvfPower();
  const double epsilon = 0.001; // Precision on decimal double value
  for (int i = 0; i < simulationDatas->vvvfPowers.size(); i++) {
    if (std::abs(simulationDatas->vvvfPowers[i] - maxVvvfPower) < epsilon) {
      maxPowTime += simulationDatas->time[i];
    }
  }
  return maxPowTime;
}

int OutputHandler::getAllDataNumber() {
  return (std::min({simulationDatas->trainSpeeds.size(),
                    simulationDatas->vvvfPowers.size(),
                    simulationDatas->catenaryPowers.size(),
                    simulationDatas->vvvfCurrents.size(),
                    simulationDatas->catenaryCurrents.size(),
                    simulationDatas->time.size(),
                    simulationDatas->timeTotal.size(),
                    simulationDatas->distance.size(),
                    simulationDatas->distanceTotal.size(),
                    simulationDatas->accelerations.size(),
                    simulationDatas->accelerationsSi.size(),
                    simulationDatas->motorForce.size(),
                    simulationDatas->motorResistance.size(),
                    simulationDatas->totalResistance.size(),
                    simulationDatas->tractionForcePerMotor.size(),
                    simulationDatas->resistancePerMotor.size(),
                    simulationDatas->torque.size(),
                    simulationDatas->rpm.size(),
                    simulationDatas->powerWheel.size(),
                    simulationDatas->powerMotorOut.size(),
                    simulationDatas->powerMotorIn.size(),
                    simulationDatas->vvvfPowers.size(),
                    simulationDatas->catenaryPowers.size(),
                    simulationDatas->catenaryCurrents.size(),
                    simulationDatas->vvvfCurrents.size(),
                    simulationDatas->energyConsumptions.size(),
                    simulationDatas->energyPowerings.size(),
                    simulationDatas->energyAps.size(),
                    simulationDatas->energyCatenaries.size()}));
}

int OutputHandler::getTrainSpeedDataNumber() {
  return std::min({simulationDatas->trainSpeeds.size(),
                   simulationDatas->vvvfPowers.size(),
                   simulationDatas->catenaryPowers.size(),
                   simulationDatas->vvvfCurrents.size(),
                   simulationDatas->catenaryCurrents.size()});
}

int OutputHandler::getTractionEffortDataNumber() {
  return std::min({simulationDatas->trainSpeeds.size(),
                   simulationDatas->vvvfPowers.size(),
                   simulationDatas->catenaryPowers.size(),
                   simulationDatas->vvvfCurrents.size(),
                   simulationDatas->catenaryCurrents.size()});
}

int OutputHandler::getTrainPowerDataNumber() {
  return std::min({simulationDatas->trainSpeeds.size(),
                   simulationDatas->vvvfPowers.size(),
                   simulationDatas->catenaryPowers.size(),
                   simulationDatas->vvvfCurrents.size(),
                   simulationDatas->catenaryCurrents.size()});
}

int OutputHandler::getTrainTrackDataNumber() {
  return std::min({simulationDatas->time.size(),
                   simulationDatas->timeTotal.size(),
                   simulationDatas->distance.size(),
                   simulationDatas->distanceTotal.size()});
}
