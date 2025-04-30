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
  m_utilityHandler = new UtilityHandler(context);
  m_resistanceHandler = new ResistanceHandler(context);
  m_tractionMotorHandler = new TractionMotorHandler(context);
  m_tractiveEffortHandler = new TractiveEffortHandler(context);
  m_simulationTrackHandler = new SimulationTrackHandler(context);
  m_currentHandler = new CurrentHandler(context);
  m_csvVariableHandler = new CsvVariableHandler(context, m_simulationWarnings);
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
  emit simulationStarted();
  clearWarnings();
  m_utilityHandler->clearSimulationDatas();
  initData();
  QString phase = "Starting";
  int i = 0;
  // int notch = -2;
  Notch notch = Accelerating;
  int stationIndex = 0;
  float time = 0;
  int coastingCount = 0;
  double previousSpeed;
  double mileage = 0;
  double stationDistance = 0;
  double trainStopTime = 0;
  const double WAIT_TIME = 10.0;
  double brakingDistance = 0.0;

  if (stationData->n_station > stationData->x_station.size() + 1) {
    m_simulationWarnings.insert(
        "Number of stations exceeds the number of station data.");
  } else if (stationData->n_station < 2) {
    MessageBoxWidget messageBox("Error!", "Number of station is invalid",
                                MessageBoxWidget::Warning);
    return;
  }
  while ((movingData->v >= 0 && notch == AtStation) ||
         (stationIndex + 1 < stationData->n_station &&
          stationIndex < stationData->x_station.size())) {
    addStationSimulationDatas();
    addEnergySimulationDatas();
    simulationDatas.slopes.append(m_slope);
    simulationDatas.radiuses.append(m_radius);
    simulationDatas.speedLimits.append(m_maxSpeed);
    simulationDatas.mileages.append(mileage);
    resistanceData->f_resStart =
        m_resistanceHandler->calculateStartRes(m_slope, m_radius);
    resistanceData->f_resRunning = m_resistanceHandler->calculateRunningRes(
        movingData->v, m_slope, m_radius);
    mileage = m_simulationTrackHandler->calculateMileage(movingData->v);
    brakingDistance =
        m_simulationTrackHandler->calculateBrakingTrack(movingData->v);
    simulationDatas.brakingDistances.append(brakingDistance);
    if (notch == AtStation) {
      phase = "At Station";
      notch = AtStation;
      movingData->v = 0;
      movingData->v_si = 0;
      movingData->acc = 0;
      movingData->acc_si = 0;
      energyData->e_aps = 0;
      energyData->e_motor = 0;
      trainStopTime += constantData->dt;
      time += constantData->dt;
      simulationDatas.time.append(constantData->dt);
      stationData->x_odo = 0.0;
      if (trainStopTime >= WAIT_TIME) {
        trainStopTime = 0;
        stationIndex++;
        notch = Accelerating;
      }
      simulationDatas.accelerations.append(movingData->acc);
      simulationDatas.accelerationsSi.append(movingData->acc_si);
      simulationDatas.trainSpeeds.append(movingData->v);
      simulationDatas.trainSpeedsSi.append(movingData->v_si);
    } else if (mileage < stationData->x_station[stationIndex] &&
               notch != Braking) {
      if (notch == Accelerating) {
        if (movingData->v >= m_maxSpeed && resistanceData->f_total > 0) {
          phase = "Coasting";
          notch = Coasting;
          continue;
        }
        phase = "Accelerating";
        m_tractiveEffortHandler->calculatePoweringForce(movingData->acc,
                                                        movingData->v);
        m_tractiveEffortHandler->calculateTotalForce(movingData->v);
        movingData->acc_si =
            (resistanceData->f_total / massData->mass_totalInertial);
        movingData->acc = constantData->cV * movingData->acc_si;
        calculatePowers(m_efficiencyGear, m_efficiencyMotor, m_efficiencyVvvf);
        movingData->v_si += movingData->acc_si * constantData->dt;
        movingData->v += movingData->acc * constantData->dt;
        simulationDatas.accelerations.append(movingData->acc);
        simulationDatas.accelerationsSi.append(movingData->acc_si);
        simulationDatas.trainSpeeds.append(movingData->v);
        simulationDatas.trainSpeedsSi.append(movingData->v_si);
        time += constantData->dt;
        simulationDatas.time.append(constantData->dt);
        energyData->e_pow += m_energyHandler->calculateEnergyOfPowering(i);
      } else if (notch == Coasting) {
        if (movingData->v <= (m_maxSpeed - movingData->v_diffCoast)) {
          coastingCount++;
          notch = Accelerating;
        }
        phase = "Coasting";
        resistanceData->f_motor = 0;
        resistanceData->f_total = -resistanceData->f_resRunning;
        movingData->acc_si =
            (resistanceData->f_total / massData->mass_totalInertial);
        movingData->acc = constantData->cV * movingData->acc_si;
        calculatePowers(m_efficiencyGear, m_efficiencyMotor, m_efficiencyVvvf);
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
      notch = Braking;
      m_tractiveEffortHandler->calculateBrakingForce();
      resistanceData->f_brake =
          m_tractiveEffortHandler->calculateTotalBrakeForce();
      movingData->decc_si =
          resistanceData->f_brake / massData->mass_totalInertial;
      movingData->decc = constantData->cV * movingData->decc_si;
      calculatePowers(m_efficiencyGear, m_efficiencyMotor, m_efficiencyVvvf);
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
        // Explicitly set speed to zero (in case it went negative)
        movingData->v = 0;
        movingData->v_si = 0;

        // Record all data for this final braking iteration
        energyData->e_motor += m_energyHandler->calculateEnergyConsumption(i);
        energyData->e_aps += m_energyHandler->calculateEnergyOfAps(i);
        phase == "Braking" ? energyData->e_catenary +=
                             m_energyHandler->calculateEnergyRegeneration(i)
                           : energyData->e_catenary +=
                             m_energyHandler->calculateEnergyOfPowering(i);

        movingData->x = abs(calculateTotalDistance(i));
        stationData->x_odo = 0; // We're at the station
        movingData->x_total += movingData->x;

        // Calculate all other required values
        trainMotorData->tm_f_res =
            m_tractionMotorHandler->calculateResistanceForcePerMotor(
                resistanceData->f_resStart);
        trainMotorData->tm_f = m_tractionMotorHandler->calculateTractionForce();
        trainMotorData->tm_t = m_tractionMotorHandler->calculateTorque();
        trainMotorData->tm_rpm = m_tractionMotorHandler->calculateRpm();

        energyData->curr_catenary =
            m_currentHandler->calculateCatenaryCurrent(m_lineVoltage);
        energyData->curr_vvvf =
            m_currentHandler->calculateVvvfCurrent(m_lineVoltage);

        // Record the data point with the final braking state
        m_utilityHandler->addSimulationDatas(i, time, phase);
        i++; // Important - increment i before continuing

        notch = AtStation;
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
    energyData->e_motor += m_energyHandler->calculateEnergyConsumption(i);
    energyData->e_aps += m_energyHandler->calculateEnergyOfAps(i);
    phase == "Braking" ? energyData->e_catenary +=
                         m_energyHandler->calculateEnergyRegeneration(i)
                       : energyData->e_catenary +=
                         m_energyHandler->calculateEnergyOfPowering(i);
    movingData->x = abs(calculateTotalDistance(i));
    stationData->x_odo =
        notch == AtStation ? 0 : stationData->x_odo + movingData->x;
    movingData->x_total += movingData->x;
    trainMotorData->tm_f_res =
        m_tractionMotorHandler->calculateResistanceForcePerMotor(
            movingData->v > 0 ? resistanceData->f_resRunning
                              : resistanceData->f_resStart);
    trainMotorData->tm_f = m_tractionMotorHandler->calculateTractionForce();
    trainMotorData->tm_t = m_tractionMotorHandler->calculateTorque();
    trainMotorData->tm_rpm = m_tractionMotorHandler->calculateRpm();

    energyData->curr_catenary =
        m_currentHandler->calculateCatenaryCurrent(m_lineVoltage);
    energyData->curr_vvvf =
        m_currentHandler->calculateVvvfCurrent(m_lineVoltage);

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
  double maxVvvfCurrent = 0.0;
  double maxVvvfPower = 0.0;
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
    if (energyData->curr_vvvf > maxVvvfCurrent) {
      maxVvvfCurrent = energyData->curr_vvvf;
    }
    if (powerData->p_motorOut > maxVvvfPower) {
      maxVvvfPower = powerData->p_vvvfIn;
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

void TrainSimulation::addEnergySimulationDatas() {

  m_efficiencyVvvf =
      m_csvVariableHandler->setEffVvvfData(m_effVvvfIndex, movingData->v);
  m_efficiencyGear =
      m_csvVariableHandler->setEffGearData(m_effGearIndex, movingData->v);
  m_efficiencyMotor =
      m_csvVariableHandler->setEffMotorData(m_effMotorIndex, movingData->v);
  m_lineVoltage = m_csvVariableHandler->setLineVoltageData(m_lineVoltageIndex,
                                                           movingData->v);
  m_motorVoltage = m_csvVariableHandler->setMotorVoltageData(
      m_motorVoltageIndex, movingData->v);

  m_effVvvfIndex =
      m_csvVariableHandler->setEffVvvfIndex(m_effVvvfIndex, movingData->v);
  m_effGearIndex =
      m_csvVariableHandler->setEffGearIndex(m_effGearIndex, movingData->v);
  m_effMotorIndex =
      m_csvVariableHandler->setEffMotorIndex(m_efficiencyMotor, movingData->v);
  m_lineVoltageIndex = m_csvVariableHandler->setLineVoltageIndex(
      m_lineVoltageIndex, movingData->v);
  m_motorVoltageIndex = m_csvVariableHandler->setMotorVoltageIndex(
      m_motorVoltageIndex, movingData->v);
}

void TrainSimulation::addStationSimulationDatas() {
  m_slope =
      m_csvVariableHandler->setSlopeData(m_slopeIndex, movingData->x_total);
  m_radius =
      m_csvVariableHandler->setRadiusData(m_radiusIndex, movingData->x_total);
  m_maxSpeed = m_csvVariableHandler->setMaxSpeedData(m_maxSpeedIndex,
                                                     movingData->x_total);
  m_slopeIndex =
      m_csvVariableHandler->setSlopeIndex(m_slopeIndex, movingData->x_total);
  m_radiusIndex =
      m_csvVariableHandler->setRadiusIndex(m_radiusIndex, movingData->x_total);
  m_maxSpeedIndex = m_csvVariableHandler->setMaxSpeedIndex(m_maxSpeedIndex,
                                                           movingData->x_total);
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
