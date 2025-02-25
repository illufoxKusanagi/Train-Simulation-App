#include "train_simulation.h"
#include <qdebug.h>

void initTrainMassData() {
  loadData.n_M1 = 3;
  loadData.n_M2 = 3;
  massData.m_M1 = 20;
  massData.m_M2 = 20;
  loadData.n_Tc = 2;
  massData.m_TC = 10;
  loadData.n_T1 = 2;
  massData.m_T1 = 10;
  loadData.n_T2 = 1;
  massData.m_T2 = 10;
  loadData.n_T3 = 1;
  massData.m_T3 = 10;
  loadData.n_PM1 = 200;
  loadData.n_PM2 = 200;
  loadData.n_PTc = 100;
  loadData.n_PT1 = 200;
  loadData.n_PT2 = 200;
  loadData.n_PT3 = 200;
  massData.i_M = 1.2;
  massData.i_T = 1.2;
  loadData.m_P = 70;
  trainData.n_car = 12.0;
}

void initData() {
  initTrainMassData();
  resistanceData.startRes = 39.2;
  resistanceData.radius = 2000;
  resistanceData.slope = 0;
  movingData.acc_start = 1.0;
  movingData.decc_start = 1.0;
  movingData.v_limit = 70.0;
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
}

double countMassEmptyCar() {
  massData.m_Me =
      (loadData.n_M1 * massData.m_M1) + (loadData.n_M2 * massData.m_M2);
  massData.m_Te =
      (loadData.n_Tc * massData.m_TC) * (loadData.n_T1 * massData.m_T1) +
      (loadData.n_T2 * massData.m_T2) + (loadData.n_T3 * massData.m_T3);
  massData.m_totalEmpty = massData.m_Me + massData.m_Te;
  return massData.m_totalEmpty;
}

double countMassWithLoad() {
  loadData.m_P = loadData.m_P / 1000;
  massData.m_M = massData.m_Me +
                 loadData.n_M1 * (loadData.m_P * loadData.n_PM1) +
                 massData.m_M2 * (loadData.m_P * loadData.n_PM2);
  massData.m_T = massData.m_Te +
                 loadData.n_Tc * (loadData.m_P * loadData.n_PTc) +
                 loadData.n_T1 * (loadData.m_P * loadData.n_PT1) +
                 loadData.n_T2 * (loadData.m_P * loadData.n_PT2) +
                 loadData.n_T3 * (loadData.m_P * loadData.n_PT3);
  massData.m_totalLoad = massData.m_M + massData.m_T;
  return massData.m_totalLoad;
}

double countMassLoadInput() {
  massData.m_totalLoad =
      massData.m_totalEmpty + trainData.n_car * loadData.load;
  return massData.m_totalLoad;
}

double countInertialMass() {
  massData.m_Mi = (massData.m_Me * massData.i_M) +
                  (loadData.n_M1 * (loadData.m_P * loadData.n_PM1) +
                   loadData.n_M2 * (loadData.m_P * loadData.n_PM2));
  massData.m_Ti = (massData.m_Te * massData.i_M) +
                  (loadData.n_Tc * (loadData.m_P * loadData.n_PTc) +
                   loadData.n_T1 * (loadData.m_P * loadData.n_PT1) +
                   loadData.n_T2 * (loadData.m_P * loadData.n_PT2) +
                   loadData.n_T3 * (loadData.m_P * loadData.n_PT3));
  massData.m_totalInertial = massData.m_Mi + massData.m_Ti;
  return massData.m_totalInertial;
}

double countInertialMassInput() {
  massData.m_totalInertial = (massData.m_Me * massData.i_M) +
                             (massData.m_Te * massData.i_T) +
                             (trainData.n_car * loadData.load);
  return massData.m_totalInertial;
}

double calculateResTrain(float m, float startRes) {
  resistanceData.f_res = m * startRes;
  return resistanceData.f_res;
}

double calculateResSlope(float m, float slope) {
  resistanceData.f_res = m * slope;
  return resistanceData.f_res;
}

double calculateResRadius(float m, float radius) {
  resistanceData.f_res = m * radius;
  return resistanceData.f_res;
}

double calculateStartForce(float acc) {
  resistanceData.f_start = massData.m_totalInertial * (acc / constantData.cV) +
                           resistanceData.f_resStart;
  return resistanceData.f_start;
}

double calculateBrakeForce() {
  resistanceData.f_brake =
      massData.m_totalInertial * (movingData.decc_start / constantData.cV);
  return resistanceData.f_brake;
}

double calculateTotalForce(float v) {
  resistanceData.f_total =
      resistanceData.f_motor -
      (v <= 0 ? resistanceData.f_resStart : resistanceData.f_resRunning);
  return resistanceData.f_total;
}

double calculateTotalBrakeForce() {
  resistanceData.f_total = resistanceData.f_motor;
  return resistanceData.f_total;
}

double calculateTractionForce() {
  trainMotorData.tm_f = resistanceData.f_motor / trainData.n_tm;
  return trainMotorData.tm_f;
}

double calculateTorque() {
  trainMotorData.tm_t =
      (resistanceData.f_motor * (trainData.wheel / 2000)) / trainData.gearRatio;
  return trainMotorData.tm_t;
}

double calculateRpm() {
  trainMotorData.tm_rpm = (movingData.v * 1000 / 60 * trainData.gearRatio) /
                          ((M_PI * trainData.wheel) / 1000);
  return trainMotorData.tm_rpm;
}

double calcualteAdhesion() {
  trainMotorData.tm_adh =
      (trainMotorData.tm_f * constantData.cF) /
      (((massData.m_M1 + massData.m_M2) * 1000) / trainData.n_axle);
  return trainMotorData.tm_adh;
}

double calculateResistanceForcePerMotor(double f_res) {
  trainMotorData.tm_f_res = f_res / (loadData.n_M1 + loadData.n_M2);
  return trainMotorData.tm_f_res;
}

double calculatePowerWheel() {
  powerData.p_wheel =
      abs(resistanceData.f_motor) * (movingData.v / constantData.cV);
  return powerData.p_wheel;
}

double calculateOutputTractionMotor() {
  powerData.p_motorOut = powerData.p_wheel / efficiencyData.eff_gear;
  return powerData.p_motorOut;
}

double calculateInputTractionMotor() {
  powerData.p_motorIn = powerData.p_motorOut / efficiencyData.eff_motor;
  return powerData.p_motorIn;
}

double calculatePowerInputOfVvvf() {
  powerData.p_vvvfIn = powerData.p_motorIn / efficiencyData.eff_vvvf;
  return powerData.p_vvvfIn;
}

double calculatePowerOfCatenary() {
  powerData.p_catenary = powerData.p_vvvfIn * efficiencyData.eff_vvvf;
  return powerData.p_catenary;
}

void calculatePoweringForce(float acc, float v) {
  if (v <= 0)
    resistanceData.f_start = calculateStartForce(movingData.acc_start);
  if (v <= movingData.v_p1) {
    resistanceData.f_motor = resistanceData.f_start;
  } else if (v > movingData.v_p1 && v <= movingData.v_p2) {
    resistanceData.f_motor = (resistanceData.f_start * movingData.v_p1) / v;
  } else if (v > movingData.v_p2) {
    resistanceData.f_motor =
        (resistanceData.f_start * movingData.v_p1 * movingData.v_p2) / (v * v);
  }
}

void calculateBrakingForce() {
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

void simulateStaticTrainMovement() {
  movingData.v_limit = 130;
  int i = 0;
  bool isAccelerating = true;
  bool isCoasting = false;
  float time = 0;
  QString phase;
  int coastingCount = 0;
  phase = "Accelerating";
  while (movingData.v <= movingData.v_limit + 1) {
    resistanceData.f_resStart = calculateStartForce(movingData.acc_start);
    resistanceData.f_resRunning =
        calculateResTrain(massData.m_totalInertial, movingData.v);
    calculatePoweringForce(movingData.acc, movingData.v);
    calculateTotalForce(movingData.v);
    trainMotorData.tm_f_res =
        calculateResistanceForcePerMotor(resistanceData.f_res);
    trainMotorData.tm_f = calculateTractionForce();
    trainMotorData.tm_t = calculateTorque();
    movingData.acc =
        constantData.cV * resistanceData.f_total / massData.m_totalInertial;
    movingData.v++;
    trainMotorData.tm_rpm = calculateRpm();
    if (i == 0) {
      trainMotorData.tm_adh = calcualteAdhesion();
    }
    qDebug() << "Phase: " << phase << " Iteration: " << i + 1
             << " Time: " << time << " Velocity: " << movingData.v
             << " Acceleration: " << movingData.acc
             << " Motor Force: " << resistanceData.f_motor << " Resistance: "
             << (movingData.v > 0 ? resistanceData.f_resRunning
                                  : resistanceData.f_resStart)
             << " Total Force: " << resistanceData.f_total
             << " Traction Force: " << trainMotorData.tm_f
             << " Resistance Force: " << trainMotorData.tm_f_res
             << " Power Wheel: " << powerData.p_wheel
             << " Power Motor Out: " << powerData.p_motorOut
             << " Power Motor In: " << powerData.p_motorIn
             << " Power Vvvf In: " << powerData.p_vvvfIn
             << " Power Catenary: " << powerData.p_catenary;
  }
}

void simulateDynamicTrainMovement() {
  int i = 0;
  bool isAccelerating = true;
  bool isCoasting = false;
  float time = 0;
  QString phase;
  int coastingCount = 0;
  while (movingData.v >= 0) {
    resistanceData.f_resStart = calculateStartForce(movingData.acc_start);
    resistanceData.f_resRunning =
        calculateResTrain(massData.m_totalInertial, movingData.v);
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
      }
      phase = "Coasting";
      calculatePoweringForce(movingData.acc, movingData.v);
      calculateTotalForce(movingData.v);
      movingData.acc =
          constantData.cV * resistanceData.f_total / massData.m_totalInertial;
      movingData.v += movingData.acc * constantData.dt;
    }
    if (i == 0) {
      qDebug() << "Phase: " << phase << " Iteration: " << i << " Time: " << time
               << " Velocity: " << 0 << " Acceleration: " << movingData.acc
               << " Motor Force: " << resistanceData.f_motor
               << " Resistance: " << resistanceData.f_resStart
               << " Total Force: " << resistanceData.f_total
               << " Traction Force: " << trainMotorData.tm_f
               << " Resistance Force: " << trainMotorData.tm_f_res
               << " Power Wheel: " << powerData.p_wheel
               << " Power Motor Out: " << powerData.p_motorOut
               << " Power Motor In: " << powerData.p_motorIn
               << " Power Vvvf In: " << powerData.p_vvvfIn
               << " Power Catenary: " << powerData.p_catenary;
    }
    time += constantData.dt;
    qDebug() << "Phase: " << phase << " Iteration: " << i + 1
             << " Time: " << time << " Velocity: " << movingData.v
             << " Acceleration: " << movingData.acc
             << " Motor Force: " << resistanceData.f_motor << " Resistance: "
             << (movingData.v > 0 ? resistanceData.f_resRunning
                                  : resistanceData.f_resStart)
             << " Total Force: " << resistanceData.f_total
             << " Traction Force: " << trainMotorData.tm_f
             << " Resistance Force: " << trainMotorData.tm_f_res
             << " Power Wheel: " << powerData.p_wheel
             << " Power Motor Out: " << powerData.p_motorOut
             << " Power Motor In: " << powerData.p_motorIn
             << " Power Vvvf In: " << powerData.p_vvvfIn
             << " Power Catenary: " << powerData.p_catenary;
  }
}
