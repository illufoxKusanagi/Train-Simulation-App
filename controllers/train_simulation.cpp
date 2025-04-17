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
  m_utilityHandler->clearSimulationDatas();
  initData();
  int i = 0;
  int j = 0;
  bool isAccelerating = true;
  bool isCoasting = false;
  bool isAtStation = false;
  bool isBraking = false;
  float time = 0;
  QString phase = "Starting";
  int coastingCount = 0;
  double previousSpeed;
  double mileage = 0;
  double stationDistance = 0;
  double trainStopTime = 0;
  const double WAIT_TIME = 10.0;
  movingData->x_station = 2000;
  while (movingData->v >= 0 || j < stationData->stationDistance.size()) {
    resistanceData->f_resStart = m_resistanceHandler->calculateStartRes();
    resistanceData->f_resRunning = m_resistanceHandler->calculateRunningRes(
        movingData->v, resistanceData->slope);
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
        j++;
        isAccelerating = true;
        isCoasting = false;
      }
      simulationDatas.accelerations.append(movingData->acc);
      simulationDatas.accelerationsSi.append(movingData->acc_si);
      simulationDatas.trainSpeeds.append(movingData->v);
      simulationDatas.trainSpeedsSi.append(movingData->v_si);
    } else if (mileage < stationData->stationDistance[j]) {
      if (isAccelerating) {
        if (movingData->v >= movingData->v_limit &&
            resistanceData->f_total > 0) {
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
        calculatePowers();
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
        if (movingData->v <= (movingData->v_limit - movingData->v_diffCoast)) {
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
        calculatePowers();
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
      calculatePowers();
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

    energyData->curr_catenary = m_currentHandler->calculateCatenaryCurrent();
    energyData->curr_vvvf = m_currentHandler->calculateVvvfCurrent();

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
  while (movingData->v <= movingData->v_limit) {
    resistanceData->f_resStart = m_resistanceHandler->calculateStartRes();
    phase = "Accelerating";
    resistanceData->f_resRunning = m_resistanceHandler->calculateRunningRes(
        movingData->v, resistanceData->slope);
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
    calculatePowers();
    energyData->curr_catenary = m_currentHandler->calculateCatenaryCurrent();
    energyData->curr_vvvf = m_currentHandler->calculateVvvfCurrent();
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
  if (stationData->stationDistance.size() > 0) {
    return true;
  } else
    return false;
}

void TrainSimulation::calculatePowers() {
  powerData->p_wheel = m_powerHandler->calculatePowerWheel();
  powerData->p_motorOut = m_powerHandler->calculateOutputTractionMotor();
  powerData->p_motorIn = m_powerHandler->calculateInputTractionMotor();
  powerData->p_vvvfIn = m_powerHandler->calculatePowerInputOfVvvf();
  powerData->p_catenary = m_powerHandler->calculatePowerOfCatenary();
  trainMotorData->tm_rpm = m_tractionMotorHandler->calculateRpm();
}

void TrainSimulation::calculateEnergies(int i) {
  energyData->e_motor += m_energyHandler->calculateEnergyConsumption(i);
  energyData->e_pow += m_energyHandler->calculateEnergyOfPowering(i);
  energyData->e_aps += m_energyHandler->calculateEnergyOfAps(i);
}