#include "train_simulation.h"
#include <chrono>
#include <qdebug.h>

TrainSimulation::TrainSimulation(QObject *parent) : QObject(parent) {
  initData();
}

void TrainSimulation::initTrainMassData() {
  loadData.n_M1 = 3;
  loadData.n_M2 = 3;
  massData.m_M1 = 20.0;
  massData.m_M2 = 20.0;
  loadData.n_Tc = 2;
  massData.m_TC = 10.0;
  loadData.n_T1 = 2;
  massData.m_T1 = 10.0;
  loadData.n_T2 = 1;
  massData.m_T2 = 10.0;
  loadData.n_T3 = 1;
  massData.m_T3 = 10.0;
  loadData.n_PM1 = 200;
  loadData.n_PM2 = 200;
  loadData.n_PTc = 100;
  loadData.n_PT1 = 200;
  loadData.n_PT2 = 200;
  loadData.n_PT3 = 200;
  massData.i_M = 1.2;
  massData.i_T = 1.2;
  loadData.m_P = 70.0;
  trainData.n_car = 12.0;
}

void TrainSimulation::initData() {
  initTrainMassData();
  massData.m_totalEmpty = countMassEmptyCar();
  massData.m_totalLoad = countMassWithLoad();
  massData.m_totalInertial = countInertialMass();
  resistanceData.startRes = 39.2;
  resistanceData.radius = 2000;
  resistanceData.slope = 0.0;
  movingData.acc_start = 1.0;
  movingData.decc_start = 1.0;
  movingData.v_limit = 70.0;
  movingData.v_diffCoast = 5.0;
  movingData.v = 0.0;
  movingData.v_p1 = 35;
  movingData.v_p2 = 65;
  movingData.v_b1 = 55;
  movingData.v_b2 = 70;
  trainData.gearRatio = 3.0;
  trainData.wheel = 500.0;
  efficiencyData.eff_gear = 0.98;
  efficiencyData.eff_motor = 0.89;
  efficiencyData.eff_vvvf = 0.96;
  powerData.p_aps = 30;
  trainData.n_axle = 4;
  trainData.n_tm = 24;
}

double TrainSimulation::countMassEmptyCar() {
  massData.m_Me =
      (loadData.n_M1 * massData.m_M1) + (loadData.n_M2 * massData.m_M2);
  massData.m_Te =
      (loadData.n_Tc * massData.m_TC) + (loadData.n_T1 * massData.m_T1) +
      (loadData.n_T2 * massData.m_T2) + (loadData.n_T3 * massData.m_T3);
  return massData.m_Me + massData.m_Te;
}

double TrainSimulation::countMassWithLoad() {
  loadData.m_P = loadData.m_P / 1000;
  massData.m_M = massData.m_Me +
                 loadData.n_M1 * (loadData.m_P * loadData.n_PM1) +
                 loadData.n_M2 * (loadData.m_P * loadData.n_PM2);
  massData.m_T = massData.m_Te +
                 loadData.n_Tc * (loadData.m_P * loadData.n_PTc) +
                 loadData.n_T1 * (loadData.m_P * loadData.n_PT1) +
                 loadData.n_T2 * (loadData.m_P * loadData.n_PT2) +
                 loadData.n_T3 * (loadData.m_P * loadData.n_PT3);
  return massData.m_M + massData.m_T;
}

double TrainSimulation::countMassLoadInput() {
  massData.m_totalLoad =
      massData.m_totalEmpty + trainData.n_car * loadData.load;
  return massData.m_totalLoad;
}

double TrainSimulation::countInertialMass() {
  massData.m_Mi = (massData.m_Me * massData.i_M) +
                  (loadData.n_M1 * (loadData.m_P * loadData.n_PM1) +
                   loadData.n_M2 * (loadData.m_P * loadData.n_PM2));
  massData.m_Ti = (massData.m_Te * massData.i_M) +
                  (loadData.n_Tc * (loadData.m_P * loadData.n_PTc) +
                   loadData.n_T1 * (loadData.m_P * loadData.n_PT1) +
                   loadData.n_T2 * (loadData.m_P * loadData.n_PT2) +
                   loadData.n_T3 * (loadData.m_P * loadData.n_PT3));
  return massData.m_Mi + massData.m_Ti;
}

double TrainSimulation::countInertialMassInput() {
  massData.m_totalInertial = (massData.m_Me * massData.i_M) +
                             (massData.m_Te * massData.i_T) +
                             (trainData.n_car * loadData.load);
  return massData.m_totalInertial;
}

double TrainSimulation::calculateResTrain(float m, float startRes) {
  resistanceData.f_res = (m * startRes) / 1000;
  return resistanceData.f_res;
}

double TrainSimulation::calculateResSlope(float m, float slope) {
  resistanceData.f_res = (m * constantData.g * slope / 1000);
  return resistanceData.f_res;
}

double TrainSimulation::calculateResRadius(float m, float radius) {
  resistanceData.f_res = (m * constantData.g * (6 / radius)) / 1000;
  return resistanceData.f_res;
}

double TrainSimulation::calculateStartForce(float acc) {
  resistanceData.f_start = massData.m_totalInertial * (acc / constantData.cV) +
                           resistanceData.f_resStart;
  return resistanceData.f_start;
}

double TrainSimulation::calculateBrakeForce() {
  resistanceData.f_brake =
      massData.m_totalInertial * (movingData.decc_start / constantData.cV);
  return resistanceData.f_brake;
}

double TrainSimulation::calculateTotalForce(float v) {
  resistanceData.f_total =
      resistanceData.f_motor -
      (v <= 0 ? resistanceData.f_resStart : resistanceData.f_resRunning);
  return resistanceData.f_total;
}

double TrainSimulation::calculateTotalBrakeForce() {
  resistanceData.f_total = resistanceData.f_motor;
  return resistanceData.f_total;
}

double TrainSimulation::calculateTractionForce() {
  trainMotorData.tm_f = resistanceData.f_motor / trainData.n_tm;
  return trainMotorData.tm_f;
}

double TrainSimulation::calculateTorque() {
  trainMotorData.tm_t =
      (resistanceData.f_motor * (trainData.wheel / 2000)) / trainData.gearRatio;
  return trainMotorData.tm_t;
}

double TrainSimulation::calculateRpm() {
  trainMotorData.tm_rpm = (movingData.v * 1000 / 60 * trainData.gearRatio) /
                          ((M_PI * trainData.wheel) / 1000);
  return trainMotorData.tm_rpm;
}

double TrainSimulation::calculateAdhesion() {
  trainMotorData.tm_adh =
      (trainMotorData.tm_f * constantData.cF) /
      (((massData.m_M1 + massData.m_M2) * 1000) / trainData.n_axle);
  return trainMotorData.tm_adh;
}

double TrainSimulation::calculateResistanceForcePerMotor(double f_res) {
  trainMotorData.tm_f_res = f_res / (loadData.n_M1 + loadData.n_M2);
  return trainMotorData.tm_f_res;
}

double TrainSimulation::calculatePowerWheel() {
  powerData.p_wheel =
      abs(resistanceData.f_motor) * (movingData.v / constantData.cV);
  return powerData.p_wheel;
}

double TrainSimulation::calculateOutputTractionMotor() {
  powerData.p_motorOut = powerData.p_wheel / efficiencyData.eff_gear;
  return powerData.p_motorOut;
}

double TrainSimulation::calculateInputTractionMotor() {
  powerData.p_motorIn = powerData.p_motorOut / efficiencyData.eff_motor;
  return powerData.p_motorIn;
}

double TrainSimulation::calculatePowerInputOfVvvf() {
  powerData.p_vvvfIn = powerData.p_motorIn / efficiencyData.eff_vvvf;
  return powerData.p_vvvfIn;
}

double TrainSimulation::calculatePowerOfCatenary() {
  powerData.p_catenary = powerData.p_vvvfIn * efficiencyData.eff_vvvf;
  return powerData.p_catenary;
}

double TrainSimulation::calculateStartRes() {
  resistanceData.r_train =
      calculateResTrain(massData.m_totalLoad, resistanceData.startRes);
  resistanceData.r_slope =
      calculateResSlope(massData.m_totalInertial, resistanceData.slope);
  resistanceData.r_radius =
      calculateResRadius(massData.m_totalInertial, resistanceData.radius);
  return resistanceData.r_train + resistanceData.r_slope +
         resistanceData.r_radius;
}

double TrainSimulation::calculateRunningRes(float v) {
  resistanceData.r_slope =
      calculateResSlope(massData.m_totalInertial, resistanceData.slope);
  resistanceData.r_radius =
      calculateResRadius(massData.m_totalInertial, resistanceData.radius);
  resistanceData.r_run =
      (1.0 / 1000.0) *
      (((1.65 + (0.0247 * v)) * (massData.m_M * constantData.g)) +
       ((0.78 + (0.0028 * v)) * (massData.m_T * constantData.g)) +
       (constantData.g * (0.028 + 0.0078 * (trainData.n_car - 1)) * (v * v)));
  return resistanceData.r_run + resistanceData.r_slope +
         resistanceData.r_radius;
}

void TrainSimulation::calculatePoweringForce(float acc, float v) {
  if (v <= 0)
    resistanceData.f_start = calculateStartForce(acc);
  if (v <= movingData.v_p1) {
    resistanceData.f_motor = resistanceData.f_start;
  } else if (v > movingData.v_p1 && v <= movingData.v_p2) {
    resistanceData.f_motor = (resistanceData.f_start * movingData.v_p1) / v;
  } else if (v > movingData.v_p2) {
    resistanceData.f_motor =
        (resistanceData.f_start * movingData.v_p1 * movingData.v_p2) / (v * v);
  }
}

void TrainSimulation::calculateBrakingForce() {
  resistanceData.f_brake =
      massData.m_totalInertial * (movingData.decc_start / constantData.cV);
  if (movingData.v < movingData.v_b1) {
    resistanceData.f_motor = -resistanceData.f_brake;
  } else if (movingData.v > movingData.v_b1 &&
             movingData.v <= movingData.v_b2) {
    resistanceData.f_motor =
        -((resistanceData.f_brake * movingData.v_b1) / movingData.v);
  } else if (movingData.v > movingData.v_b2) {
    resistanceData.f_motor =
        -((resistanceData.f_brake * movingData.v_b1 * movingData.v_b2) /
          (movingData.v * movingData.v));
  } else {
    qDebug() << "Invalid input";
  }
}

void TrainSimulation::simulateDynamicTrainMovement() {
  auto startTime = std::chrono::high_resolution_clock::now();
  int i = 0;
  bool isAccelerating = true;
  bool isCoasting = false;
  float time = 0;
  QString phase;
  int coastingCount = 0;
  movingData.v = 0.0;
  movingData.acc = movingData.acc_start;
  while (movingData.v >= 0) {
    resistanceData.f_resStart = calculateStartRes();
    resistanceData.f_resRunning = calculateRunningRes(movingData.v);
    if (isAccelerating) {
      if (movingData.v >= movingData.v_limit) {
        isAccelerating = false;
        isCoasting = true;
        phase = "Coasting";
        continue;
      }
      phase = "Accelerating";
      calculatePoweringForce(movingData.acc, movingData.v);
      calculateTotalForce(movingData.v);
      movingData.acc =
          constantData.cV * resistanceData.f_total / massData.m_totalInertial;
      movingData.v += movingData.acc * constantData.dt;
    } else if (isCoasting) {
      if (movingData.v <= (movingData.v_limit - movingData.v_diffCoast)) {
        isCoasting = false;
        isAccelerating = true;
        coastingCount++;
        if (coastingCount >= 3) {
          isAccelerating = false;
          isCoasting = false;
          continue;
        }
      }
      phase = "Coasting";
      resistanceData.f_motor = 0;
      resistanceData.f_total = -resistanceData.f_resRunning;
      movingData.acc =
          constantData.cV * resistanceData.f_total / massData.m_totalInertial;
      movingData.v += movingData.acc * constantData.dt;
    } else {
      phase = "Braking";
      calculateBrakingForce();
      resistanceData.f_brake = calculateTotalBrakeForce();

      movingData.decc =
          constantData.cV * resistanceData.f_total / massData.m_totalInertial;
      movingData.v += movingData.decc * constantData.dt;
      if (movingData.v <= 0)
        break;
    }
    trainMotorData.tm_f_res = calculateResistanceForcePerMotor(
        movingData.v > 0 ? resistanceData.f_resRunning
                         : resistanceData.f_resStart);
    trainMotorData.tm_f = calculateTractionForce();
    trainMotorData.tm_t = calculateTorque();
    trainMotorData.tm_rpm = calculateRpm();
    powerData.p_wheel = calculatePowerWheel();
    powerData.p_motorOut = calculateOutputTractionMotor();
    powerData.p_motorIn = calculateInputTractionMotor();
    powerData.p_vvvfIn = calculatePowerInputOfVvvf();
    powerData.p_catenary = calculatePowerOfCatenary();

    if (i == 0)
      trainMotorData.tm_adh = calculateAdhesion();
    qDebug() << "Phase: Starting " << "\nIteration: " << i << "\nTime: " << time
             << "\nVelocity: " << 0 << "\nAcceleration: " << movingData.acc
             << "\nMotor Force: " << resistanceData.f_motor
             << "\nSpeed: " << movingData.v << "\nResistance: "
             << (movingData.v > 0 ? resistanceData.f_resRunning
                                  : resistanceData.f_resStart)
             << "\nTotal Force: " << resistanceData.f_total
             << "\nTraction Force: " << trainMotorData.tm_f
             << "\nResistance Force: " << trainMotorData.tm_f_res
             << "\nPower Wheel: " << powerData.p_wheel
             << "\nPower Motor Out: " << powerData.p_motorOut
             << "\nPower Motor In: " << powerData.p_motorIn
             << "\nPower Vvvf In: " << powerData.p_vvvfIn
             << "\nPower Catenary: " << powerData.p_catenary;
    time += constantData.dt;
    qDebug() << "Phase: " << phase << "\nIteration: " << i + 1
             << "\nTime: " << time << "\nVelocity: " << movingData.v
             << "\nAcceleration: "
             << (phase == "Braking" ? movingData.decc : movingData.acc)
             << "\nMotor Force: " << resistanceData.f_motor
             << "\nSpeed: " << movingData.v << "\nResistance: "
             << (movingData.v > 0 ? resistanceData.f_resRunning
                                  : resistanceData.f_resStart)
             << "\nTotal Force: " << resistanceData.f_total
             << "\nTraction Force: " << trainMotorData.tm_f
             << "\nResistance Force: " << trainMotorData.tm_f_res
             << "\nPower Wheel: " << powerData.p_wheel
             << "\nPower Motor Out: " << powerData.p_motorOut
             << "\nPower Motor In: " << powerData.p_motorIn
             << "\nPower Vvvf In: " << powerData.p_vvvfIn
             << "\nPower Catenary: " << powerData.p_catenary;
    i++;
  }
  auto endTime = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      endTime - startTime);
  qDebug() << "Simulation completed in " << (duration.count() / 1000.0)
           << " seconds";
  emit simulationCompleted();
}

void TrainSimulation::simulateStaticTrainMovement() {
  movingData.v_limit = 130;
  int i = 0;
  bool isAccelerating = true;
  bool isCoasting = false;
  QString phase = "Starting";
  int CoastingCount = 0;
  float time = 0.0;
  movingData.v = 0.0;
  movingData.acc = movingData.acc_start;
  while (movingData.v <= movingData.v_limit + 1) {
    phase = "Accelerating";
    resistanceData.f_resStart = calculateStartRes();
    resistanceData.f_resRunning = calculateRunningRes(movingData.v);
    calculatePoweringForce(movingData.acc, movingData.v);
    resistanceData.f_total = calculateTotalForce(movingData.v);
    trainMotorData.tm_f_res =
        calculateResistanceForcePerMotor(resistanceData.f_res);
    trainMotorData.tm_f = calculateTractionForce();
    trainMotorData.tm_t = calculateTorque();
    movingData.acc =
        constantData.cV * resistanceData.f_total / massData.m_totalInertial;
    movingData.v += 0.5;
    powerData.p_wheel = calculatePowerWheel();
    powerData.p_motorOut = calculateOutputTractionMotor();
    powerData.p_motorIn = calculateInputTractionMotor();
    powerData.p_vvvfIn = calculatePowerInputOfVvvf();
    powerData.p_catenary = calculatePowerOfCatenary();
    trainMotorData.tm_rpm = calculateRpm();
    if (i == 0)
      trainMotorData.tm_adh = calculateAdhesion();
    qDebug() << "Phase: Starting " << "\nIteration: " << i << "\nTime: " << time
             << "\nVelocity: " << 0 << "\nAcceleration: " << movingData.acc
             << "\nMotor Force: " << resistanceData.f_motor
             << "\nSpeed: " << movingData.v << "\nResistance: "
             << (movingData.v > 0 ? resistanceData.f_resRunning
                                  : resistanceData.f_resStart)
             << "\nTotal Force: " << resistanceData.f_total
             << "\nTraction Force: " << trainMotorData.tm_f
             << "\nResistance Force: " << trainMotorData.tm_f_res;
    time += constantData.dt;
    qDebug() << "Phase: " << phase << "\nIteration: " << i + 1
             << "\nTime: " << time << "\nVelocity: " << movingData.v
             << "\nAcceleration: " << movingData.acc
             << "\nMotor Force: " << resistanceData.f_motor
             << "\nSpeed: " << movingData.v << "\nResistance: "
             << (movingData.v > 0 ? resistanceData.f_resRunning
                                  : resistanceData.f_resStart)
             << "\nTotal Force: " << resistanceData.f_total
             << "\nTraction Force: " << trainMotorData.tm_f
             << "\nResistance Force: " << trainMotorData.tm_f_res
             << "\nPower Wheel: " << powerData.p_wheel
             << "\nPower Motor Out: " << powerData.p_motorOut
             << "\nPower Motor In: " << powerData.p_motorIn
             << "\nPower Vvvf In: " << powerData.p_vvvfIn
             << "\nPower Catenary: " << powerData.p_catenary;
    i++;
  }
  emit simulationCompleted();
}