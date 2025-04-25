#include "train_simulation.h"

using namespace std;

TrainSimulation::TrainSimulation(AppContext &context, QObject *parent)
    : QObject(parent), trainData(context.trainData.data()),
      massData(context.massData.data()), loadData(context.loadData.data()),
      resistanceData(context.resistanceData.data()),
      movingData(context.movingData.data()),
      trainMotorData(context.trainMotorData.data()),
      efficiencyData(context.efficiencyData.data()),
      powerData(context.powerData.data()),
      energyData(context.energyData.data()),
      stationData(context.stationData.data()),
      simulationDatas(*context.simulationData.data()),
      constantData(context.constantData.data()) {
  m_massHandler = new MassHandler(context);
  m_outputHandler = new OutputHandler(simulationDatas);
  m_energyHandler = new EnergyHandler(context);
  m_powerHandler = new PowerHandler(context);
  m_utilityHandler = new UtilityHandler(context, simulationDatas);
  m_resistanceHandler = new ResistanceHandler(context);
  m_tractionMotorHandler = new TractionMotorHandler(context);
  m_tractiveEffortHandler = new TractiveEffortHandler(context);
  m_simulationTrackHandler = new SimulationTrackHandler(context);
  m_currentHandler = new CurrentHandler(context);
  initData();
  connect(this, &TrainSimulation::simulationCompleted, m_utilityHandler,
          &UtilityHandler::resetSimulation);
  connect(this, &TrainSimulation::staticSimulationCompleted, m_utilityHandler,
          &UtilityHandler::resetSimulation);
}

void TrainSimulation::initData() {
  massData->mass_totalEmpty = m_massHandler->countMassEmptyCar();
  if (loadData->load > 0) {
    massData->mass_totalLoad = m_massHandler->countMassLoadInput();
    massData->mass_totalInertial = m_massHandler->countInertialMassInput();
  } else {
    massData->mass_totalLoad = m_massHandler->countMassWithLoad();
    massData->mass_totalInertial = m_massHandler->countInertialMass();
  }
  movingData->v = 0.0;
  movingData->acc = movingData->acc_start;
  movingData->decc = movingData->decc_start * constantData->cV;
}

double TrainSimulation::calculateTotalTime(int i) {
  if (i <= 0)
    return 0;
  return ((simulationDatas.trainSpeeds[i] -
           simulationDatas.trainSpeeds[i - 1]) /
          constantData->cV) /
         (simulationDatas.accelerations[i - 1] / constantData->cV);
}

double TrainSimulation::calculateTotalDistance(int i) {
  if (i <= 0)
    return 0;
  else
    return ((simulationDatas.trainSpeeds[i - 1] / constantData->cV) *
            simulationDatas.time[i]) +
           (0.5 * (simulationDatas.accelerations[i - 1] / constantData->cV) *
            pow(simulationDatas.time[i], 2));
}

void TrainSimulation::simulateDynamicTrainMovement() {
  clearWarnings();
  m_utilityHandler->clearSimulationDatas();
  initData();
  QString phase = "Starting";
  int i = 0;
  int stationIndex = 0;
  bool isAccelerating = true;
  bool isCoasting = false;
  bool isAtStation = false;
  bool isBraking = false;
  float time = 0;
  int coastingCount = 0;
  double previousSpeed;
  double mileage = 0;
  double stationDistance = 0;
  double trainStopTime = 0;
  const double WAIT_TIME = 10.0;
  int slopeIndex = 0;
  int radiusIndex = 0;
  int maxSpeedIndex = 0;
  int effVvvfIndex = 0;
  int effGearIndex = 0;
  int effMotorIndex = 0;
  int lineVoltageIndex = 0;
  int motorVoltageIndex = 0;
  double slope = stationData->stat_slope;
  double radius = stationData->stat_radius;
  double maxSpeed = stationData->stat_v_limit;
  double lineVoltage = energyData->stat_vol_line;
  double motorVoltage = energyData->stat_vol_motor;
  double efficiencyVvvf = efficiencyData->stat_eff_vvvf;
  double efficiencyGear = efficiencyData->stat_eff_gear;
  double efficiencyMotor = efficiencyData->stat_eff_motor;
  if (stationData->n_station > stationData->x_station.size() + 1) {
    m_simulationWarnings.insert(
        "Number of stations exceeds the number of station data.");
  }
  while (movingData->v >= 0 || (stationIndex + 2 < stationData->n_station &&
                                stationIndex < stationData->x_station.size())) {
    slope = setSlopeData(slopeIndex, movingData->x_total);
    radius = setRadiusData(radiusIndex, movingData->x_total);
    maxSpeed = setMaxSpeedData(maxSpeedIndex, movingData->x_total);
    efficiencyVvvf = setEffVvvfData(effVvvfIndex, movingData->v);
    efficiencyGear = setEffGearData(effGearIndex, movingData->v);
    efficiencyMotor = setEffMotorData(effMotorIndex, movingData->v);
    lineVoltage = setLineVoltageData(lineVoltageIndex, movingData->v);
    motorVoltage = setMotorVoltageData(motorVoltageIndex, movingData->v);
    slopeIndex = setSlopeIndex(slopeIndex, movingData->x_total);
    radiusIndex = setRadiusIndex(radiusIndex, movingData->x_total);
    maxSpeedIndex = setMaxSpeedIndex(maxSpeedIndex, movingData->x_total);
    effVvvfIndex = setEffVvvfIndex(effVvvfIndex, movingData->v);
    effGearIndex = setEffGearIndex(effGearIndex, movingData->v);
    simulationDatas.slopes.append(slope);
    simulationDatas.radiuses.append(radius);
    simulationDatas.speedLimits.append(maxSpeed);
    resistanceData->f_resStart =
        m_resistanceHandler->calculateStartRes(slope, radius);
    resistanceData->f_resRunning =
        m_resistanceHandler->calculateRunningRes(movingData->v, slope, radius);
    mileage = m_simulationTrackHandler->calculateMileage();
    if (isAtStation) {
      phase = "At Station";
      movingData->v = 0;
      movingData->v_si = 0;
      movingData->acc = 0;
      movingData->acc_si = 0;
      energyData->e_aps = 0;
      energyData->e_motor = 0;
      trainStopTime += constantData->dt;
      time += constantData->dt;
      simulationDatas.time.append(constantData->dt);
      if (trainStopTime >= WAIT_TIME) {
        isAtStation = false;
        trainStopTime = 0;
        stationIndex++;
        isAccelerating = true;
        isCoasting = false;
      }
      simulationDatas.accelerations.append(movingData->acc);
      simulationDatas.accelerationsSi.append(movingData->acc_si);
      simulationDatas.trainSpeeds.append(movingData->v);
      simulationDatas.trainSpeedsSi.append(movingData->v_si);
    } else if (mileage < stationData->x_station[stationIndex]) {
      if (isAccelerating) {
        if (movingData->v >= maxSpeed && resistanceData->f_total > 0) {
          isAccelerating = false;
          isCoasting = true;
          phase = "Coasting";
          continue;
        }
        phase = "Accelerating";
        m_tractiveEffortHandler->calculatePoweringForce(movingData->acc,
                                                        movingData->v);
        m_tractiveEffortHandler->calculateTotalForce(movingData->v);
        movingData->acc_si =
            (resistanceData->f_total / massData->mass_totalInertial);
        movingData->acc = constantData->cV * movingData->acc_si;
        calculatePowers(efficiencyGear, efficiencyMotor, efficiencyVvvf);
        movingData->v_si += movingData->acc_si * constantData->dt;
        movingData->v += movingData->acc * constantData->dt;
        simulationDatas.accelerations.append(movingData->acc);
        simulationDatas.accelerationsSi.append(movingData->acc_si);
        simulationDatas.trainSpeeds.append(movingData->v);
        simulationDatas.trainSpeedsSi.append(movingData->v_si);
        time += constantData->dt;
        simulationDatas.time.append(constantData->dt);
        energyData->e_pow += m_energyHandler->calculateEnergyOfPowering(i);
      } else if (isCoasting) {
        if (movingData->v <= (maxSpeed - movingData->v_diffCoast)) {
          isCoasting = false;
          isAccelerating = true;
          coastingCount++;
        }
        phase = "Coasting";
        resistanceData->f_motor = 0;
        resistanceData->f_total = -resistanceData->f_resRunning;
        movingData->acc_si =
            (resistanceData->f_total / massData->mass_totalInertial);
        movingData->acc = constantData->cV * movingData->acc_si;
        calculatePowers(efficiencyGear, efficiencyMotor, efficiencyVvvf);
        movingData->v_si += movingData->acc_si * constantData->dt;
        movingData->v += movingData->acc * constantData->dt;
        simulationDatas.accelerations.append(movingData->acc);
        simulationDatas.accelerationsSi.append(movingData->acc_si);
        simulationDatas.trainSpeeds.append(movingData->v);
        simulationDatas.trainSpeedsSi.append(movingData->v_si);
        time += constantData->dt;
        simulationDatas.time.append(constantData->dt);
        energyData->e_pow += m_energyHandler->calculateEnergyOfPowering(i);
      }
    } else {
      phase = "Braking";
      m_tractiveEffortHandler->calculateBrakingForce();
      resistanceData->f_brake =
          m_tractiveEffortHandler->calculateTotalBrakeForce();
      movingData->decc_si =
          resistanceData->f_brake / massData->mass_totalInertial;
      movingData->decc = constantData->cV * movingData->decc_si;
      calculatePowers(efficiencyGear, efficiencyMotor, efficiencyVvvf);
      movingData->v_si += movingData->decc_si * constantData->dt;
      movingData->v += movingData->decc * constantData->dt;
      simulationDatas.accelerations.append(movingData->decc);
      simulationDatas.accelerationsSi.append(movingData->decc_si);
      simulationDatas.trainSpeeds.append(movingData->v);
      simulationDatas.trainSpeedsSi.append(movingData->v_si);
      time += constantData->dt;
      simulationDatas.time.append(constantData->dt);
      energyData->e_reg += m_energyHandler->calculateEnergyRegeneration(i);
      if (movingData->v <= 0) {
        isAtStation = true;
        trainStopTime = 0;
        continue;
      }
      if (resistanceData->f_total == 0) {
        MessageBoxWidget messageBox("Error",
                                    "Total force is unable to move the train.",
                                    MessageBoxWidget::Warning);
        break;
      }
    }
    simulationDatas.mileages.append(mileage);
    energyData->e_motor += m_energyHandler->calculateEnergyConsumption(i);
    energyData->e_aps += m_energyHandler->calculateEnergyOfAps(i);
    phase == "Braking" ? energyData->e_catenary +=
                         m_energyHandler->calculateEnergyRegeneration(i)
                       : energyData->e_catenary +=
                         m_energyHandler->calculateEnergyOfPowering(i);
    movingData->x = abs(calculateTotalDistance(i));
    movingData->x_total += movingData->x;
    trainMotorData->tm_f_res =
        m_tractionMotorHandler->calculateResistanceForcePerMotor(
            movingData->v > 0 ? resistanceData->f_resRunning
                              : resistanceData->f_resStart);
    trainMotorData->tm_f = m_tractionMotorHandler->calculateTractionForce();
    trainMotorData->tm_t = m_tractionMotorHandler->calculateTorque();
    trainMotorData->tm_rpm = m_tractionMotorHandler->calculateRpm();

    energyData->curr_catenary =
        m_currentHandler->calculateCatenaryCurrent(lineVoltage);
    energyData->curr_vvvf = m_currentHandler->calculateVvvfCurrent(lineVoltage);

    m_utilityHandler->addSimulationDatas(i, time, phase);
    if (i == 0) {
      trainMotorData->tm_adh = m_tractiveEffortHandler->calculateAdhesion();
    }
    i++;
  }
  emit simulationCompleted();
}

void TrainSimulation::simulateStaticTrainMovement() {
  m_utilityHandler->clearSimulationDatas();
  initData();
  double v_limit = 130;
  int i = 0;
  bool isAccelerating = true;
  bool isCoasting = false;
  QString phase = "Starting";
  int CoastingCount = 0;
  float time = 0.0;
  while (movingData->v <= stationData->stat_v_limit &&
         movingData->x_total < stationData->stat_x_station) {
    resistanceData->f_resStart = m_resistanceHandler->calculateStartRes(
        stationData->stat_slope, stationData->stat_radius);
    phase = "Accelerating";
    resistanceData->f_resRunning = m_resistanceHandler->calculateRunningRes(
        movingData->v, stationData->stat_slope, stationData->stat_radius);
    calculateRunningResEachSlope();
    m_tractiveEffortHandler->calculatePoweringForce(movingData->acc,
                                                    movingData->v);
    resistanceData->f_total =
        m_tractiveEffortHandler->calculateTotalForce(movingData->v);
    trainMotorData->tm_f_res =
        m_tractionMotorHandler->calculateResistanceForcePerMotor(
            movingData->v > 0 ? resistanceData->f_resRunning
                              : resistanceData->f_resStart);
    trainMotorData->tm_f = m_tractionMotorHandler->calculateTractionForce();
    trainMotorData->tm_t = m_tractionMotorHandler->calculateTorque();
    movingData->acc_si = resistanceData->f_total / massData->mass_totalInertial;
    movingData->acc = constantData->cV * movingData->acc_si;

    simulationDatas.accelerations.append(movingData->acc);
    simulationDatas.accelerationsSi.append(movingData->acc_si);
    simulationDatas.trainSpeeds.append(movingData->v);
    simulationDatas.trainSpeedsSi.append(movingData->v_si);

    movingData->time = abs(calculateTotalTime(i));
    movingData->time_total += movingData->time;
    simulationDatas.time.append(movingData->time);
    movingData->x = abs(calculateTotalDistance(i));
    movingData->x_total += movingData->x;
    calculatePowers(efficiencyData->stat_eff_gear,
                    efficiencyData->stat_eff_motor,
                    efficiencyData->stat_eff_vvvf);
    energyData->curr_catenary =
        m_currentHandler->calculateCatenaryCurrent(energyData->stat_vol_line);
    energyData->curr_vvvf =
        m_currentHandler->calculateVvvfCurrent(energyData->stat_vol_line);
    calculateEnergies(i);
    movingData->v++;
    movingData->v_si = movingData->v / constantData->cV;

    m_utilityHandler->addSimulationDatas(i, movingData->time_total, phase);
    if (i == 0) {
      trainMotorData->tm_adh = m_tractiveEffortHandler->calculateAdhesion();
    }
    if (energyData->curr_vvvf > m_maxVvvfCurrent) {
      m_maxVvvfCurrent = energyData->curr_vvvf;
    }
    if (powerData->p_motorOut > m_maxVvvfPower) {
      m_maxVvvfPower = powerData->p_vvvfIn;
    }
    i++;
  }
  emit staticSimulationCompleted();
}

double TrainSimulation::getMaxSpeed() {
  if (simulationDatas.trainSpeeds.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas.trainSpeeds.begin(),
                           simulationDatas.trainSpeeds.end());
}

double TrainSimulation::getMaxVvvfPower() {
  if (simulationDatas.vvvfPowers.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas.vvvfPowers.begin(),
                           simulationDatas.vvvfPowers.end());
}

double TrainSimulation::getMaxCatenaryPower() {
  if (simulationDatas.catenaryPowers.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas.catenaryPowers.begin(),
                           simulationDatas.catenaryPowers.end());
}

double TrainSimulation::getMaxVvvfCurrent() {
  if (simulationDatas.vvvfCurrents.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas.vvvfCurrents.begin(),
                           simulationDatas.vvvfCurrents.end());
}

double TrainSimulation::getMaxCatenaryCurrent() {
  if (simulationDatas.catenaryCurrents.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas.catenaryCurrents.begin(),
                           simulationDatas.catenaryCurrents.end());
}

double TrainSimulation::getMaxTractionEffort() {
  if (simulationDatas.tractionEfforts.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas.tractionEfforts.begin(),
                           simulationDatas.tractionEfforts.end());
}

double TrainSimulation::getDistanceTravelled() {
  if (simulationDatas.distanceTotal.isEmpty())
    return 0.0;
  return simulationDatas.distanceTotal.last();
}

double TrainSimulation::getMaxEnergyConsumption() {
  if (simulationDatas.energyConsumptions.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas.energyConsumptions.begin(),
                           simulationDatas.energyConsumptions.end());
}

double TrainSimulation::getMaxEnergyRegen() {
  if (simulationDatas.energyRegenerations.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas.energyRegenerations.begin(),
                           simulationDatas.energyRegenerations.end());
}

double TrainSimulation::getMaxEnergyPowering() {
  if (simulationDatas.energyPowerings.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas.energyPowerings.begin(),
                           simulationDatas.energyPowerings.end());
}

double TrainSimulation::getMaxEnergyAps() {
  if (simulationDatas.energyAps.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas.energyAps.begin(),
                           simulationDatas.energyAps.end());
}

double TrainSimulation::getMaxCurrTime() {
  double maxCurrTime = 0.0;
  double maxVvvfCurrent = getMaxVvvfCurrent();
  const double epsilon = 0.001; // Precision on decimal double value
  for (int i = 0; i < simulationDatas.vvvfCurrents.size(); i++) {
    if (std::abs(simulationDatas.vvvfCurrents[i] - maxVvvfCurrent) < epsilon) {
      maxCurrTime += simulationDatas.time[i];
    }
  }
  return maxCurrTime;
}

double TrainSimulation::getMaxPowTime() {
  double maxPowTime = 0.0;
  double maxVvvfPower = getMaxVvvfPower();
  const double epsilon = 0.001; // Precision on decimal double value
  for (int i = 0; i < simulationDatas.vvvfPowers.size(); i++) {
    if (std::abs(simulationDatas.vvvfPowers[i] - maxVvvfPower) < epsilon) {
      maxPowTime += simulationDatas.time[i];
    }
  }
  return maxPowTime;
}

double TrainSimulation::getAdhesion() { return trainMotorData->tm_adh; }

bool TrainSimulation::validateCsvVariables() {
  if (stationData->x_station.size() > 0) {
    return true;
  } else
    return false;
}

void TrainSimulation::calculatePowers(double efficiencyGear,
                                      double efficiencyMotor,
                                      double efficiencyVvvf) {
  powerData->p_wheel = m_powerHandler->calculatePowerWheel();
  powerData->p_motorOut =
      m_powerHandler->calculateOutputTractionMotor(efficiencyGear);
  powerData->p_motorIn =
      m_powerHandler->calculateInputTractionMotor(efficiencyMotor);
  powerData->p_vvvfIn =
      m_powerHandler->calculatePowerInputOfVvvf(efficiencyVvvf);
  powerData->p_catenary = m_powerHandler->calculatePowerOfCatenary();
  trainMotorData->tm_rpm = m_tractionMotorHandler->calculateRpm();
}

void TrainSimulation::calculateEnergies(int i) {
  energyData->e_motor += m_energyHandler->calculateEnergyConsumption(i);
  energyData->e_pow += m_energyHandler->calculateEnergyOfPowering(i);
  energyData->e_aps += m_energyHandler->calculateEnergyOfAps(i);
}

int TrainSimulation::setSlopeIndex(int slopeIndex, double distanceTravelled) {
  if (!stationData->slope.empty()) {
    if (distanceTravelled >= stationData->x_slopeEnd[slopeIndex]) {
      slopeIndex++;
    }
    return slopeIndex;
  } else if (slopeIndex >= stationData->slope.size()) {
    return slopeIndex - 1;
  }
  return 0;
}

int TrainSimulation::setRadiusIndex(int radiusIndex, double distanceTravelled) {
  if (!stationData->radius.empty()) {
    if (distanceTravelled >= stationData->x_radiusEnd[radiusIndex]) {
      radiusIndex++;
    }
    return radiusIndex;
  } else if (radiusIndex >= stationData->radius.size()) {
    return radiusIndex - 1;
  }
  return 0;
}

int TrainSimulation::setMaxSpeedIndex(int maxSpeedIndex,
                                      double distanceTravelled) {
  if (!stationData->v_limit.empty()) {
    if (distanceTravelled >= stationData->x_v_limitEnd[maxSpeedIndex]) {
      maxSpeedIndex++;
    }
    return maxSpeedIndex;
  } else if (maxSpeedIndex >= stationData->v_limit.size()) {
    return maxSpeedIndex - 1;
  }
  return 0;
}

int TrainSimulation::setEffGearIndex(int effGearIndex, double speed) {
  if (!efficiencyData->eff_gear.empty()) {
    if (speed >= efficiencyData->v_eff_gear[effGearIndex]) {
      effGearIndex++;
    } else if (speed < efficiencyData->v_eff_gear[effGearIndex] &&
               effGearIndex > 0) {
      return effGearIndex - 1;
    }
    return effGearIndex;
  }
  return 0;
}

int TrainSimulation::setEffVvvfIndex(int effVvvfIndex, double speed) {
  if (!efficiencyData->eff_vvvf.empty()) {
    if (speed >= efficiencyData->v_eff_vvvf[effVvvfIndex]) {
      effVvvfIndex++;
    } else if (speed < efficiencyData->v_eff_vvvf[effVvvfIndex] &&
               effVvvfIndex > 0) {
      return effVvvfIndex - 1;
    }
    return effVvvfIndex;
  }
  return 0;
}

int TrainSimulation::setEffMotorIndex(int effMotorIndex, double speed) {
  if (!efficiencyData->eff_motor.empty()) {
    if (speed >= efficiencyData->v_eff_motor[effMotorIndex]) {
      effMotorIndex++;
    } else if (speed < efficiencyData->v_eff_motor[effMotorIndex] &&
               effMotorIndex > 0) {
      return effMotorIndex - 1;
    }
    return effMotorIndex;
  }
  return 0;
}

int TrainSimulation::setLineVoltageIndex(int lineVoltageIndex, double speed) {
  if (!energyData->vol_line.empty()) {
    if (speed >= energyData->v_vol_line[lineVoltageIndex]) {
      lineVoltageIndex++;
    } else if (speed < energyData->v_vol_line[lineVoltageIndex] &&
               lineVoltageIndex > 0) {
      return lineVoltageIndex - 1;
    }
    return lineVoltageIndex;
  }
  return 0;
}

int TrainSimulation::setMotorVoltageIndex(int motorVoltageIndex, double speed) {
  if (!energyData->vol_motor.empty()) {
    if (speed >= energyData->v_vol_motor[motorVoltageIndex]) {
      motorVoltageIndex++;
    } else if (speed < energyData->v_vol_motor[motorVoltageIndex] &&
               motorVoltageIndex > 0) {
      return motorVoltageIndex - 1;
    }
    return motorVoltageIndex;
  }
  return 0;
}

double TrainSimulation::setSlopeData(int slopeIndex, double distanceTravelled) {
  if (!stationData->slope.empty() && slopeIndex < stationData->slope.size()) {
    if (distanceTravelled >= stationData->x_slopeEnd[slopeIndex]) {
      return stationData->slope[slopeIndex++];
    } else {
      return stationData->slope[slopeIndex];
    }
  } else if (stationData->slope.empty()) {
    m_simulationWarnings.insert("Slope csv data is empty.");
  } else {
    m_simulationWarnings.insert("Attempted to access slope data beyond range.");
  }
  return stationData->stat_slope;
}

double TrainSimulation::setRadiusData(int radiusIndex,
                                      double distanceTravelled) {
  if (!stationData->radius.empty() &&
      radiusIndex < stationData->radius.size()) {
    if (distanceTravelled >= stationData->x_radiusEnd[radiusIndex]) {
      return stationData->radius[radiusIndex++];
    } else {
      return stationData->radius[radiusIndex];
    }
  } else if (stationData->radius.empty()) {
    m_simulationWarnings.insert("Radius csv data is empty.");
  } else {
    m_simulationWarnings.insert(
        "Attempted to access radius data beyond range.");
  }
  return stationData->stat_radius;
}

double TrainSimulation::setMaxSpeedData(int maxSpeedIndex,
                                        double distanceTravelled) {
  if (!stationData->v_limit.empty() &&
      maxSpeedIndex < stationData->v_limit.size()) {
    if (distanceTravelled >= stationData->x_v_limitEnd[maxSpeedIndex]) {
      return stationData->v_limit[maxSpeedIndex++];
    } else {
      return stationData->v_limit[maxSpeedIndex];
    }
  } else if (stationData->v_limit.empty()) {
    m_simulationWarnings.insert("Speed limit csv data is empty.");
  } else {
    m_simulationWarnings.insert(
        "Attempted to access max speed data beyond range.");
  }
  return stationData->stat_v_limit;
}

double TrainSimulation::setEffVvvfData(int effVvvfIndex, double speed) {
  if (!efficiencyData->eff_vvvf.empty() &&
      effVvvfIndex < efficiencyData->eff_vvvf.size()) {
    if (speed >= efficiencyData->v_eff_vvvf[effVvvfIndex]) {
      return efficiencyData->eff_vvvf[effVvvfIndex++];
    } else if (speed < efficiencyData->v_eff_vvvf[effVvvfIndex] &&
               effVvvfIndex > 0) {
      return efficiencyData->eff_vvvf[effVvvfIndex - 1];
    } else {
      return efficiencyData->eff_vvvf[effVvvfIndex];
    }
  } else if (efficiencyData->eff_vvvf.empty()) {
    m_simulationWarnings.insert("VVVF efficiency csv data is empty.");
  } else {
    m_simulationWarnings.insert(
        "Attempted to access VVVF efficiency data beyond range.");
  }
  return efficiencyData->stat_eff_vvvf;
}

double TrainSimulation::setEffGearData(int effGearIndex, double speed) {
  if (!efficiencyData->eff_gear.empty() &&
      effGearIndex < efficiencyData->eff_gear.size()) {
    if (speed >= efficiencyData->v_eff_gear[effGearIndex]) {
      return efficiencyData->eff_gear[effGearIndex++];
    } else if (speed < efficiencyData->v_eff_gear[effGearIndex] &&
               effGearIndex > 0) {
      return efficiencyData->eff_gear[effGearIndex - 1];
    } else {
      return efficiencyData->eff_gear[effGearIndex];
    }
  } else if (efficiencyData->eff_gear.empty()) {
    m_simulationWarnings.insert("Gear efficiency csv data is empty.");
  } else {
    m_simulationWarnings.insert(
        "Attempted to access gear efficiency data beyond range.");
  }
  return efficiencyData->stat_eff_gear;
}

double TrainSimulation::setEffMotorData(int effMotorIndex, double speed) {
  if (!efficiencyData->eff_motor.empty() &&
      effMotorIndex < efficiencyData->eff_motor.size()) {
    if (speed >= efficiencyData->v_eff_motor[effMotorIndex]) {
      return efficiencyData->eff_motor[effMotorIndex++];
    } else if (speed < efficiencyData->v_eff_motor[effMotorIndex] &&
               effMotorIndex > 0) {
      return efficiencyData->eff_motor[effMotorIndex - 1];
    } else {
      return efficiencyData->eff_motor[effMotorIndex];
    }
  } else if (efficiencyData->eff_motor.empty()) {
    m_simulationWarnings.insert("Motor efficiency csv data is empty.");
  } else {
    m_simulationWarnings.insert(
        "Attempted to access motor efficiency data beyond range.");
  }
  return efficiencyData->stat_eff_motor;
}

double TrainSimulation::setLineVoltageData(int lineVoltageIndex, double speed) {
  if (!energyData->vol_line.empty() &&
      lineVoltageIndex < energyData->vol_line.size()) {
    if (speed >= energyData->v_vol_line[lineVoltageIndex]) {
      return energyData->vol_line[lineVoltageIndex++];
    } else if (speed < energyData->v_vol_line[lineVoltageIndex] &&
               lineVoltageIndex > 0) {
      return energyData->vol_line[lineVoltageIndex - 1];
    } else {
      return energyData->vol_line[lineVoltageIndex];
    }
  } else if (energyData->vol_line.empty()) {
    m_simulationWarnings.insert("Line voltage csv data is empty.");
  } else {
    m_simulationWarnings.insert(
        "Attempted to access line voltage data beyond range.");
  }
  return energyData->stat_vol_line;
}

double TrainSimulation::setMotorVoltageData(int motorVoltageIndex,
                                            double speed) {
  if (!energyData->vol_motor.empty() &&
      motorVoltageIndex < energyData->vol_motor.size()) {
    if (speed >= energyData->v_vol_motor[motorVoltageIndex]) {
      return energyData->vol_motor[motorVoltageIndex++];
    } else if (speed < energyData->v_vol_motor[motorVoltageIndex] &&
               motorVoltageIndex > 0) {
      return energyData->vol_motor[motorVoltageIndex - 1];
    } else {
      return energyData->vol_motor[motorVoltageIndex];
    }
  } else if (energyData->vol_motor.empty()) {
    m_simulationWarnings.insert("Motor voltage csv data is empty.");
  } else {
    m_simulationWarnings.insert(
        "Attempted to access motor voltage data beyond range.");
  }
  return energyData->stat_vol_motor;
}

void TrainSimulation::calculateRunningResEachSlope() {
  resistanceData->f_resRunningZero =
      movingData->v == 0
          ? m_resistanceHandler->calculateStartRes(0, stationData->stat_radius)
          : m_resistanceHandler->calculateRunningRes(movingData->v, 0.0,
                                                     stationData->stat_radius);
  resistanceData->f_resRunningFive =
      movingData->v == 0
          ? m_resistanceHandler->calculateStartRes(5, stationData->stat_radius)
          : m_resistanceHandler->calculateRunningRes(movingData->v, 5.0,
                                                     stationData->stat_radius);
  resistanceData->f_resRunningTen =
      movingData->v == 0
          ? m_resistanceHandler->calculateStartRes(10.0,
                                                   stationData->stat_radius)
          : m_resistanceHandler->calculateRunningRes(movingData->v, 10.0,
                                                     stationData->stat_radius);
  resistanceData->f_resRunningTwentyFive =
      movingData->v == 0
          ? m_resistanceHandler->calculateStartRes(25.0,
                                                   stationData->stat_radius)
          : m_resistanceHandler->calculateRunningRes(movingData->v, 25.0,
                                                     stationData->stat_radius);
}