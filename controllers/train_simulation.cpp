#include "train_simulation.h"

using namespace std;

TrainSimulation::TrainSimulation(
    QObject *parent, TrainData *trainData, MassData *massData,
    LoadData *loadData, ResistanceData *resistanceData, MovingData *movingData,
    TrainMotorData *trainMotorData, EfficiencyData *efficiencyData,
    PowerData *powerData, EnergyData *energyData, StationData *stationData)
    : QObject(parent), trainData(trainData), massData(massData),
      loadData(loadData), resistanceData(resistanceData),
      movingData(movingData), trainMotorData(trainMotorData),
      efficiencyData(efficiencyData), powerData(powerData),
      energyData(energyData), stationData(stationData) {
  simulationDatas = QSharedPointer<SimulationDatas>(new SimulationDatas());
  initData();
  connect(this, &TrainSimulation::simulationCompleted, this,
          &TrainSimulation::resetSimulation);
  connect(this, &TrainSimulation::staticSimulationCompleted, this,
          &TrainSimulation::resetSimulation);
}

void TrainSimulation::initData() {
  massData->mass_totalEmpty = countMassEmptyCar();
  if (loadData->load > 0) {
    massData->mass_totalLoad = countMassLoadInput();
    massData->mass_totalInertial = countInertialMassInput();
  } else {
    massData->mass_totalLoad = countMassWithLoad();
    massData->mass_totalInertial = countInertialMass();
  }
  movingData->v = 0.0;
  movingData->acc = movingData->acc_start;
  movingData->decc = movingData->decc_start * constantData.cV;
}

double TrainSimulation::countMassEmptyCar() {
  massData->mass_Me = (loadData->n_M1 * massData->mass_M1) +
                      (loadData->n_M2 * massData->mass_M2);
  massData->mass_Te = (loadData->n_Tc * massData->mass_TC) +
                      (loadData->n_T1 * massData->mass_T1) +
                      (loadData->n_T2 * massData->mass_T2) +
                      (loadData->n_T3 * massData->mass_T3);
  return massData->mass_Me + massData->mass_Te;
}

double TrainSimulation::countMassWithLoad() {
  loadData->mass_P_final = loadData->mass_P / 1000;
  massData->mass_M =
      massData->mass_Me +
      loadData->n_M1 * (loadData->mass_P_final * loadData->n_PM1) +
      loadData->n_M2 * (loadData->mass_P_final * loadData->n_PM2);
  massData->mass_T =
      massData->mass_Te +
      loadData->n_Tc * (loadData->mass_P_final * loadData->n_PTc) +
      loadData->n_T1 * (loadData->mass_P_final * loadData->n_PT1) +
      loadData->n_T2 * (loadData->mass_P_final * loadData->n_PT2) +
      loadData->n_T3 * (loadData->mass_P_final * loadData->n_PT3);
  return massData->mass_M + massData->mass_T;
}

double TrainSimulation::countMassLoadInput() {
  massData->mass_totalLoad =
      massData->mass_totalEmpty + trainData->n_car * loadData->load;
  return massData->mass_totalLoad;
}

double TrainSimulation::countInertialMass() {
  massData->mass_Mi =
      (massData->mass_Me * massData->i_M) +
      (loadData->n_M1 * (loadData->mass_P_final * loadData->n_PM1) +
       loadData->n_M2 * (loadData->mass_P_final * loadData->n_PM2));
  massData->mass_Ti =
      (massData->mass_Te * massData->i_T) +
      (loadData->n_Tc * (loadData->mass_P_final * loadData->n_PTc) +
       loadData->n_T1 * (loadData->mass_P_final * loadData->n_PT1) +
       loadData->n_T2 * (loadData->mass_P_final * loadData->n_PT2) +
       loadData->n_T3 * (loadData->mass_P_final * loadData->n_PT3));
  return massData->mass_Mi + massData->mass_Ti;
}

double TrainSimulation::countInertialMassInput() {
  massData->mass_totalInertial = (massData->mass_Me * massData->i_M) +
                                 (massData->mass_Te * massData->i_T) +
                                 (trainData->n_car * loadData->load);
  return massData->mass_totalInertial;
}

double TrainSimulation::calculateResTrain(float m, float startRes) {
  resistanceData->r_train = (m * startRes) / 1000;
  return resistanceData->r_train;
}

double TrainSimulation::calculateResSlope(float m, float slope) {
  resistanceData->r_slope = (m * constantData.g * slope) / 1000;
  return resistanceData->r_slope;
}

double TrainSimulation::calculateResRadius(float m, float radius) {
  resistanceData->r_radius = (m * constantData.g * (6.0 / radius)) / 1000;
  return resistanceData->r_radius;
}

double TrainSimulation::calculateStartForce(float acc) {
  return massData->mass_totalInertial * acc + resistanceData->f_resStart;
}

double TrainSimulation::calculateTotalForce(float v) {
  resistanceData->f_total =
      resistanceData->f_motor -
      (v <= 0 ? resistanceData->f_resStart : resistanceData->f_resRunning);
  return resistanceData->f_total;
}

double TrainSimulation::calculateTotalBrakeForce() {
  resistanceData->f_total = resistanceData->f_motor;
  return resistanceData->f_total;
}

double TrainSimulation::calculateTractionForce() {
  trainMotorData->tm_f = resistanceData->f_motor / trainData->n_tm;
  return trainMotorData->tm_f;
}

double TrainSimulation::calculateTorque() {
  trainMotorData->tm_t = (resistanceData->f_motor * (trainData->wheel / 2000)) /
                         trainData->gearRatio;
  return trainMotorData->tm_t;
}

double TrainSimulation::calculateRpm() {
  trainMotorData->tm_rpm = (movingData->v * 1000 / 60 * trainData->gearRatio) /
                           ((M_PI * trainData->wheel) / 1000);
  return trainMotorData->tm_rpm;
}

double TrainSimulation::calculateAdhesion() {
  trainMotorData->tm_adh =
      (trainMotorData->tm_f * constantData.cF) /
      (((massData->mass_M2 + (loadData->mass_P_final * loadData->n_PM2)) *
        1000) /
       trainData->n_axle);
  return trainMotorData->tm_adh;
}

double TrainSimulation::calculateResistanceForcePerMotor(double f_res) {
  trainMotorData->tm_f_res = f_res / trainData->n_tm;
  return trainMotorData->tm_f_res;
}

double TrainSimulation::calculatePowerWheel() {
  powerData->p_wheel =
      resistanceData->f_motor * (movingData->v / constantData.cV);
  return powerData->p_wheel;
}

double TrainSimulation::calculateOutputTractionMotor() {
  powerData->p_motorOut = powerData->p_wheel / efficiencyData->eff_gear;
  return powerData->p_motorOut;
}

double TrainSimulation::calculateInputTractionMotor() {
  powerData->p_motorIn = powerData->p_motorOut / efficiencyData->eff_motor;
  return powerData->p_motorIn;
}

double TrainSimulation::calculatePowerInputOfVvvf() {
  powerData->p_vvvfIn = powerData->p_motorIn / efficiencyData->eff_vvvf;
  return powerData->p_vvvfIn;
}

double TrainSimulation::calculatePowerOfCatenary() {
  powerData->p_catenary = powerData->p_vvvfIn + powerData->p_aps;
  return powerData->p_catenary;
}

double TrainSimulation::calculateVvvfCurrent() {
  energyData->curr_vvvf = 1000 * (powerData->p_vvvfIn / energyData->vol_line);
  return energyData->curr_vvvf;
}

double TrainSimulation::calculateCatenaryCurrent() {
  energyData->curr_catenary =
      1000 * (powerData->p_catenary / energyData->vol_line);
  return energyData->curr_catenary;
}

double TrainSimulation::calculateStartRes() {
  resistanceData->r_train =
      calculateResTrain(massData->mass_totalLoad, resistanceData->startRes);
  resistanceData->r_slope =
      calculateResSlope(massData->mass_totalInertial, resistanceData->slope);
  resistanceData->r_radius =
      calculateResRadius(massData->mass_totalInertial, resistanceData->radius);
  return resistanceData->r_train + resistanceData->r_slope +
         resistanceData->r_radius;
}

double TrainSimulation::calculateRunningRes(float v) {
  resistanceData->r_slope =
      calculateResSlope(massData->mass_totalInertial, resistanceData->slope);
  resistanceData->r_radius =
      calculateResRadius(massData->mass_totalInertial, resistanceData->radius);
  resistanceData->r_run =
      (1.0 / 1000.0) *
      (((1.65 + (0.0247 * v)) * (massData->mass_M * constantData.g)) +
       ((0.78 + (0.0028 * v)) * (massData->mass_T * constantData.g)) +
       (constantData.g * (0.028 + 0.0078 * (trainData->n_car - 1)) * (v * v)));
  return resistanceData->r_run + resistanceData->r_slope +
         resistanceData->r_radius;
}

void TrainSimulation::calculatePoweringForce(float acc, float v) {
  resistanceData->f_start = calculateStartForce(movingData->acc_start);
  if (v <= movingData->v_p1) {
    resistanceData->f_motor = resistanceData->f_start;
  } else if (v > movingData->v_p1 && v <= movingData->v_p2) {
    resistanceData->f_motor = (resistanceData->f_start * movingData->v_p1) / v;
  } else if (v > movingData->v_p2) {
    resistanceData->f_motor =
        (resistanceData->f_start * movingData->v_p1 * movingData->v_p2) /
        (v * v);
  }
}

void TrainSimulation::calculateBrakingForce() {
  resistanceData->f_brake =
      massData->mass_totalInertial * movingData->decc_start;
  if (movingData->v < movingData->v_b1) {
    resistanceData->f_motor = -resistanceData->f_brake;
  } else if (movingData->v > movingData->v_b1 &&
             movingData->v <= movingData->v_b2) {
    resistanceData->f_motor =
        -((resistanceData->f_brake * movingData->v_b1) / movingData->v);
  } else if (movingData->v > movingData->v_b2) {
    resistanceData->f_motor =
        -((resistanceData->f_brake * movingData->v_b1 * movingData->v_b2) /
          (movingData->v * movingData->v));
  } else {
    MessageBoxWidget messagebox("Error", "Braking point is invalid",
                                MessageBoxWidget::Warning);
  }
}

double TrainSimulation::calculateTotalTime(int i) {
  if (i <= 0)
    return 0;
  return ((simulationDatas->trainSpeeds[i] -
           simulationDatas->trainSpeeds[i - 1]) /
          constantData.cV) /
         (simulationDatas->accelerations[i - 1] / constantData.cV);
}

double TrainSimulation::calculateTotalDistance(int i) {
  if (i <= 0)
    return 0;
  else
    return ((simulationDatas->trainSpeeds[i - 1] / constantData.cV) *
            simulationDatas->time[i]) +
           (0.5 * (simulationDatas->accelerations[i - 1] / constantData.cV) *
            pow(simulationDatas->time[i], 2));
}

double TrainSimulation::calculateEnergyConsumption(int i) {
  if (simulationDatas->time.isEmpty())
    return 0;
  return powerData->p_motorOut / 3600 * simulationDatas->time[i];
}

double TrainSimulation::calculateEnergyOfPowering(int i) {
  if (simulationDatas->time.isEmpty())
    return 0;
  return powerData->p_catenary / 3600 * simulationDatas->time[i];
}

double TrainSimulation::calculateEnergyRegeneration(int i) {
  if (simulationDatas->time.isEmpty())
    return 0;
  return powerData->p_catenary / 3600 * simulationDatas->time[i];
}

double TrainSimulation::calculateEnergyOfAps(int i) {
  if (simulationDatas->time.isEmpty())
    return 0;
  return powerData->p_aps / 3600 * simulationDatas->time[i];
}

void TrainSimulation::simulateDynamicTrainMovement() {
  clearSimulationDatas();
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
  // stationData->stationDistance.end();
  while (movingData->v >= 0 || j < stationData->stationDistance.size()) {
    resistanceData->f_resStart = calculateStartRes();
    resistanceData->f_resRunning = calculateRunningRes(movingData->v);
    mileage = calculateMileage();
    // Add check to make sure stationData has stations
    // if (stationData->stationDistance.empty()) {
    //   // No stations defined - exit loop
    //   MessageBoxWidget messageBox("No Stations",
    //                               "No station data available for
    //                               simulation.", MessageBoxWidget::Warning);
    //   break;
    // }
    if (isAtStation) {
      phase = "At Station";
      movingData->v = 0;
      movingData->v_si = 0;
      movingData->acc = 0;
      movingData->acc_si = 0;
      energyData->e_aps = 0;
      energyData->e_motor = 0;
      trainStopTime += constantData.dt;
      time += constantData.dt;
      simulationDatas->time.append(constantData.dt);
      if (trainStopTime >= WAIT_TIME) {
        isAtStation = false;
        trainStopTime = 0;
        j++;
        isAccelerating = true;
        isCoasting = false;
      }
      simulationDatas->accelerations.append(movingData->acc);
      simulationDatas->accelerationsSi.append(movingData->acc_si);
      simulationDatas->trainSpeeds.append(movingData->v);
      simulationDatas->trainSpeedsSi.append(movingData->v_si);
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
        calculatePoweringForce(movingData->acc, movingData->v);
        calculateTotalForce(movingData->v);
        movingData->acc_si =
            (resistanceData->f_total / massData->mass_totalInertial);
        movingData->acc = constantData.cV * movingData->acc_si;
        movingData->v_si += movingData->acc_si * constantData.dt;
        movingData->v += movingData->acc * constantData.dt;
        simulationDatas->accelerations.append(movingData->acc);
        simulationDatas->accelerationsSi.append(movingData->acc_si);
        simulationDatas->trainSpeeds.append(movingData->v);
        simulationDatas->trainSpeedsSi.append(movingData->v_si);
        time += constantData.dt;
        simulationDatas->time.append(constantData.dt);
        energyData->e_pow += calculateEnergyOfPowering(i);
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
        movingData->acc = constantData.cV * movingData->acc_si;
        movingData->v_si += movingData->acc_si * constantData.dt;
        movingData->v += movingData->acc * constantData.dt;
        simulationDatas->accelerations.append(movingData->acc);
        simulationDatas->accelerationsSi.append(movingData->acc_si);
        simulationDatas->trainSpeeds.append(movingData->v);
        simulationDatas->trainSpeedsSi.append(movingData->v_si);
        time += constantData.dt;
        simulationDatas->time.append(constantData.dt);
        energyData->e_pow += calculateEnergyOfPowering(i);
      }
    } else {
      phase = "Braking";
      calculateBrakingForce();
      resistanceData->f_brake = calculateTotalBrakeForce();
      movingData->decc_si =
          resistanceData->f_brake / massData->mass_totalInertial;
      movingData->decc = constantData.cV * movingData->decc_si;
      movingData->v_si += movingData->decc_si * constantData.dt;
      movingData->v += movingData->decc * constantData.dt;
      simulationDatas->accelerations.append(movingData->decc);
      simulationDatas->accelerationsSi.append(movingData->decc_si);
      simulationDatas->trainSpeeds.append(movingData->v);
      simulationDatas->trainSpeedsSi.append(movingData->v_si);
      time += constantData.dt;
      simulationDatas->time.append(constantData.dt);
      energyData->e_reg += calculateEnergyRegeneration(i);
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
    simulationDatas->mileages.append(mileage);
    energyData->e_motor += calculateEnergyConsumption(i);
    energyData->e_aps += calculateEnergyOfAps(i);
    phase == "Braking"
        ? energyData->e_catenary += calculateEnergyRegeneration(i)
        : energyData->e_catenary += calculateEnergyOfPowering(i);
    movingData->x = abs(calculateTotalDistance(i));
    movingData->x_total += movingData->x;
    trainMotorData->tm_f_res = calculateResistanceForcePerMotor(
        movingData->v > 0 ? resistanceData->f_resRunning
                          : resistanceData->f_resStart);
    trainMotorData->tm_f = calculateTractionForce();
    trainMotorData->tm_t = calculateTorque();
    trainMotorData->tm_rpm = calculateRpm();
    powerData->p_wheel = calculatePowerWheel();
    powerData->p_motorOut = calculateOutputTractionMotor();
    powerData->p_motorIn = calculateInputTractionMotor();
    powerData->p_vvvfIn = calculatePowerInputOfVvvf();
    powerData->p_catenary = calculatePowerOfCatenary();
    energyData->curr_catenary = calculateCatenaryCurrent();
    energyData->curr_vvvf = calculateVvvfCurrent();

    addSimulationDatas(i, time, phase);
    if (i == 0) {
      trainMotorData->tm_adh = calculateAdhesion();
    }
    i++;
  }
  emit simulationCompleted();
}

void TrainSimulation::simulateStaticTrainMovement() {
  clearSimulationDatas();
  initData();
  double v_limit = 130;
  int i = 0;
  bool isAccelerating = true;
  bool isCoasting = false;
  QString phase = "Starting";
  int CoastingCount = 0;
  float time = 0.0;
  while (movingData->v <= movingData->v_limit) {
    resistanceData->f_resStart = calculateStartRes();
    phase = "Accelerating";
    resistanceData->f_resRunning = calculateRunningRes(movingData->v);
    calculatePoweringForce(movingData->acc, movingData->v);
    resistanceData->f_total = calculateTotalForce(movingData->v);
    trainMotorData->tm_f_res = calculateResistanceForcePerMotor(
        movingData->v > 0 ? resistanceData->f_resRunning
                          : resistanceData->f_resStart);
    trainMotorData->tm_f = calculateTractionForce();
    trainMotorData->tm_t = calculateTorque();
    movingData->acc_si = resistanceData->f_total / massData->mass_totalInertial;
    movingData->acc = constantData.cV * movingData->acc_si;

    simulationDatas->accelerations.append(movingData->acc);
    simulationDatas->accelerationsSi.append(movingData->acc_si);
    simulationDatas->trainSpeeds.append(movingData->v);
    simulationDatas->trainSpeedsSi.append(movingData->v_si);

    movingData->time = abs(calculateTotalTime(i));
    movingData->time_total += movingData->time;
    simulationDatas->time.append(movingData->time);
    movingData->x = abs(calculateTotalDistance(i));
    movingData->x_total += movingData->x;

    powerData->p_wheel = calculatePowerWheel();
    powerData->p_motorOut = calculateOutputTractionMotor();
    powerData->p_motorIn = calculateInputTractionMotor();
    powerData->p_vvvfIn = calculatePowerInputOfVvvf();
    powerData->p_catenary = calculatePowerOfCatenary();
    trainMotorData->tm_rpm = calculateRpm();
    energyData->curr_catenary = calculateCatenaryCurrent();
    energyData->curr_vvvf = calculateVvvfCurrent();
    energyData->e_motor += calculateEnergyConsumption(i);
    energyData->e_pow += calculateEnergyOfPowering(i);
    energyData->e_aps += calculateEnergyOfAps(i);
    movingData->v++;
    movingData->v_si = movingData->v / constantData.cV;
    addSimulationDatas(i, movingData->time_total, phase);
    if (i == 0) {
      trainMotorData->tm_adh = calculateAdhesion();
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

void TrainSimulation::addSimulationDatas(int i, double time, QString phase) {
  simulationDatas->tractionEfforts.append(resistanceData->f_motor);
  simulationDatas->vvvfPowers.append(powerData->p_vvvfIn);
  simulationDatas->catenaryPowers.append(powerData->p_catenary);
  simulationDatas->vvvfCurrents.append(energyData->curr_vvvf);
  simulationDatas->catenaryCurrents.append(energyData->curr_catenary);
  simulationDatas->distance.append(movingData->x);
  simulationDatas->timeTotal.append(time);
  simulationDatas->distanceTotal.append(movingData->x_total);
  simulationDatas->phase.append(phase);
  simulationDatas->motorForce.append(resistanceData->f_motor);
  simulationDatas->motorResistance.append(
      i == 0 ? resistanceData->f_resStart : resistanceData->f_resRunning);
  simulationDatas->totalResistance.append(resistanceData->f_total);
  simulationDatas->tractionForcePerMotor.append(trainMotorData->tm_f);
  simulationDatas->resistancePerMotor.append(trainMotorData->tm_f_res);
  simulationDatas->torque.append(trainMotorData->tm_t);
  simulationDatas->rpm.append(trainMotorData->tm_rpm);
  simulationDatas->powerWheel.append(powerData->p_wheel);
  simulationDatas->powerMotorOut.append(powerData->p_motorOut);
  simulationDatas->powerMotorIn.append(powerData->p_motorIn);
  simulationDatas->energyConsumptions.append(energyData->e_motor);
  simulationDatas->energyPowerings.append(energyData->e_pow);
  simulationDatas->energyRegenerations.append(energyData->e_reg);
  simulationDatas->energyAps.append(energyData->e_aps);
  simulationDatas->energyCatenaries.append(energyData->e_catenary);
}

void TrainSimulation::resetSimulation() {
  resistanceData->f_resStart = 0;
  resistanceData->f_resRunning = 0;
  resistanceData->f_motor = 0;
  resistanceData->f_start = 0;
  resistanceData->f_brake = 0;
  resistanceData->f_total = 0;
  resistanceData->f_total = 0;
  trainMotorData->tm_f_res = 0;
  trainMotorData->tm_f = 0;
  trainMotorData->tm_t = 0;
  movingData->acc = movingData->acc_start;
  movingData->acc_si = movingData->acc_start;
  movingData->v = 0;
  movingData->v_si = 0;
  powerData->p_wheel = 0;
  powerData->p_motorOut = 0;
  powerData->p_motorIn = 0;
  powerData->p_vvvfIn = 0;
  powerData->p_catenary = 0;
  trainMotorData->tm_rpm = 0;
  movingData->x = 0;
  movingData->time = 0;
  movingData->x_total = 0;
  movingData->time_total = 0;
  energyData->e_aps = 0;
  energyData->e_motor = 0;
  energyData->e_pow = 0;
  energyData->e_reg = 0;
  energyData->curr_catenary = 0;
  energyData->curr_vvvf = 0;
  energyData->e_catenary = 0;
}

void TrainSimulation::clearSimulationDatas() {
  simulationDatas->trainSpeeds.clear();
  simulationDatas->trainSpeedsSi.clear();
  simulationDatas->accelerations.clear();
  simulationDatas->accelerationsSi.clear();
  simulationDatas->tractionEfforts.clear();
  simulationDatas->vvvfPowers.clear();
  simulationDatas->catenaryPowers.clear();
  simulationDatas->vvvfCurrents.clear();
  simulationDatas->catenaryCurrents.clear();
  simulationDatas->phase.clear();
  simulationDatas->motorForce.clear();
  simulationDatas->motorResistance.clear();
  simulationDatas->totalResistance.clear();
  simulationDatas->tractionForcePerMotor.clear();
  simulationDatas->resistancePerMotor.clear();
  simulationDatas->torque.clear();
  simulationDatas->rpm.clear();
  simulationDatas->powerWheel.clear();
  simulationDatas->powerMotorOut.clear();
  simulationDatas->powerMotorIn.clear();
  simulationDatas->distance.clear();
  simulationDatas->time.clear();
  simulationDatas->distanceTotal.clear();
  simulationDatas->timeTotal.clear();
  simulationDatas->energyConsumptions.clear();
  simulationDatas->energyPowerings.clear();
  simulationDatas->energyRegenerations.clear();
  simulationDatas->energyAps.clear();
  simulationDatas->energyCatenaries.clear();
  simulationDatas->mileages.clear();
}

void TrainSimulation::printSimulationDatas() {
  int maxSize = getAllDataNumber();
  // Handle empty energyRegenerations array - this is the critical fix
  if (simulationDatas->energyRegenerations.size() < maxSize) {
    simulationDatas->energyRegenerations.resize(maxSize);
  }
  QString filepath = QFileDialog::getSaveFileName(
      nullptr, "Save File", QDir::homePath(), "CSV File (*.csv)");
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
             "Motor,F Res,F Total,F Motor/TM,F Res/TM,Torque,RPM,P "
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

bool TrainSimulation::saveTrainSpeedData() {
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

bool TrainSimulation::saveTractionEffortData() {
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

bool TrainSimulation::saveTrainPowerData() {
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

bool TrainSimulation::saveTrainTrackData() {
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

bool TrainSimulation::saveEnergyConsumptionData() {
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

double TrainSimulation::findMaxSpeed() {
  if (simulationDatas->trainSpeeds.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->trainSpeeds.begin(),
                           simulationDatas->trainSpeeds.end());
}

double TrainSimulation::findMaxVvvfPower() {
  if (simulationDatas->vvvfPowers.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->vvvfPowers.begin(),
                           simulationDatas->vvvfPowers.end());
}

double TrainSimulation::findMaxCatenaryPower() {
  if (simulationDatas->catenaryPowers.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->catenaryPowers.begin(),
                           simulationDatas->catenaryPowers.end());
}

double TrainSimulation::findMaxVvvfCurrent() {
  if (simulationDatas->vvvfCurrents.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->vvvfCurrents.begin(),
                           simulationDatas->vvvfCurrents.end());
}

double TrainSimulation::findMaxCatenaryCurrent() {
  if (simulationDatas->catenaryCurrents.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->catenaryCurrents.begin(),
                           simulationDatas->catenaryCurrents.end());
}

double TrainSimulation::findMaxTractionEffort() {
  if (simulationDatas->tractionEfforts.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->tractionEfforts.begin(),
                           simulationDatas->tractionEfforts.end());
}

double TrainSimulation::findDistanceTravelled() {
  if (simulationDatas->distanceTotal.isEmpty())
    return 0.0;
  return simulationDatas->distanceTotal.last();
}

double TrainSimulation::findMaxEnergyConsumption() {
  if (simulationDatas->energyConsumptions.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->energyConsumptions.begin(),
                           simulationDatas->energyConsumptions.end());
}

double TrainSimulation::findMaxEnergyRegen() {
  if (simulationDatas->energyRegenerations.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->energyRegenerations.begin(),
                           simulationDatas->energyRegenerations.end());
}

double TrainSimulation::findMaxEnergyPowering() {
  if (simulationDatas->energyPowerings.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->energyPowerings.begin(),
                           simulationDatas->energyPowerings.end());
}

double TrainSimulation::findMaxEnergyAps() {
  if (simulationDatas->energyAps.isEmpty())
    return 0.0;
  return *std::max_element(simulationDatas->energyAps.begin(),
                           simulationDatas->energyAps.end());
}

double TrainSimulation::findMaxCurrTime() {
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

double TrainSimulation::findMaxPowTime() {
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

double TrainSimulation::getAdhesion() { return trainMotorData->tm_adh; }

double TrainSimulation::calculateBrakingTrack() {
  double speed = movingData->v_limit / constantData.cV;
  double brakingTrack = (speed * constantData.t_reaction) +
                        (pow(speed, 2) / (2 * movingData->decc_start));
  return brakingTrack;
}

double TrainSimulation::calculateBrakingEmergencyTrack() {
  double speed = simulationDatas->trainSpeedsSi.last();
  double brakingTrack = (speed * constantData.t_reaction) +
                        (pow(speed, 2) / (2 * movingData->decc_emergency));
  return brakingTrack;
}

double TrainSimulation::calculateNormalSimulationTrack() {
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateBrakingTrack();
  return poweringDistance + trainLength + brakingDistance;
}

double TrainSimulation::calculateEmergencyNormalSimulationTrack() {
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateBrakingEmergencyTrack();
  return poweringDistance + trainLength + brakingDistance;
}

double TrainSimulation::calculateDelaySimulationTrack() {
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateBrakingTrack();
  return poweringDistance + brakingDistance + trainLength +
         (simulationDatas->trainSpeedsSi.last() * constantData.t_delay) +
         (0.5 * simulationDatas->accelerationsSi.last() *
          pow(constantData.t_delay, 2));
}

double TrainSimulation::calculateEmergencyDelaySimulationTrack() {
  double poweringDistance = simulationDatas->distanceTotal.last();
  double trainLength = trainData->trainsetLength;
  double brakingDistance = calculateBrakingEmergencyTrack();
  return poweringDistance + brakingDistance + trainLength +
         (simulationDatas->trainSpeedsSi.last() * constantData.t_delay) +
         (0.5 * simulationDatas->accelerationsSi.last() *
          pow(constantData.t_delay, 2));
}

double TrainSimulation::calculateSafetySimulationTrack() {
  return 1.2 * calculateDelaySimulationTrack();
}

double TrainSimulation::calculateEmergencySafetySimulationTrack() {
  return 1.2 * calculateEmergencyDelaySimulationTrack();
}

double TrainSimulation::calculateMileage() {
  double distance = simulationDatas->distanceTotal.isEmpty()
                        ? 0
                        : simulationDatas->distanceTotal.last();
  double brakingDistance = calculateBrakingTrack();
  return distance + brakingDistance;
}

int TrainSimulation::getAllDataNumber() {
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

int TrainSimulation::getTrainSpeedDataNumber() {
  return std::min({simulationDatas->trainSpeeds.size(),
                   simulationDatas->vvvfPowers.size(),
                   simulationDatas->catenaryPowers.size(),
                   simulationDatas->vvvfCurrents.size(),
                   simulationDatas->catenaryCurrents.size()});
}

int TrainSimulation::getTractionEffortDataNumber() {
  return std::min({simulationDatas->trainSpeeds.size(),
                   simulationDatas->vvvfPowers.size(),
                   simulationDatas->catenaryPowers.size(),
                   simulationDatas->vvvfCurrents.size(),
                   simulationDatas->catenaryCurrents.size()});
}

int TrainSimulation::getTrainPowerDataNumber() {
  return std::min({simulationDatas->trainSpeeds.size(),
                   simulationDatas->vvvfPowers.size(),
                   simulationDatas->catenaryPowers.size(),
                   simulationDatas->vvvfCurrents.size(),
                   simulationDatas->catenaryCurrents.size()});
}

int TrainSimulation::getTrainTrackDataNumber() {
  return std::min({simulationDatas->time.size(),
                   simulationDatas->timeTotal.size(),
                   simulationDatas->distance.size(),
                   simulationDatas->distanceTotal.size()});
}

bool TrainSimulation::validateCsvVariables() {
  if (stationData->stationDistance.size() > 0) {
    return true;
  } else
    return false;
}

void TrainSimulation::deleteCsvFile(QString csvPath) {
  QFile file(csvPath);
  if (file.exists()) {
    file.remove();
  }
}

void TrainSimulation::readCsvFile(const QString path, QStringList &values) {
  QFile file(path);
  bool isHeader = true;
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    MessageBoxWidget messagebox("Error", "Could not open file for reading",
                                MessageBoxWidget::Warning);
    return;
  }
  QTextStream in(&file);
  while (!in.atEnd()) {
    QString line = in.readLine();
    if (isHeader) {
      isHeader = false;
      continue;
    }
    values = line.split(",");
    if (values.size() >= 19) {
      double speed = values[3].toDouble();
      double tractionEffort = values[6].toDouble();
      double vvvfPower = values[15].toDouble();
      double catenaryPower = values[16].toDouble();
      double vvvfCurrent = values[18].toDouble();
      double catenaryCurrent = values[17].toDouble();
    }
  }
  file.close();
}
