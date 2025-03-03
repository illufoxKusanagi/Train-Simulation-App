#include "train_simulation.h"
#include <fstream>
#include <iostream>
#include <qdebug.h>

using namespace std;

TrainSimulation::TrainSimulation(QObject *parent, TrainData *trainData,
                                 MassData *massData, LoadData *loadData,
                                 ResistanceData *resistanceData,
                                 MovingData *movingData,
                                 TrainMotorData *trainMotorData,
                                 EfficiencyData *efficiencyData,
                                 PowerData *powerData, EnergyData *energyData)
    : QObject(parent), trainData(trainData), massData(massData),
      loadData(loadData), resistanceData(resistanceData),
      movingData(movingData), trainMotorData(trainMotorData),
      efficiencyData(efficiencyData), powerData(powerData),
      energyData(energyData)

{
  initData();
}

void TrainSimulation::initTrainMassData() {}

void TrainSimulation::initData() {
  trainData->n_car = 12.0;
  massData->m_totalEmpty = countMassEmptyCar();
  if (loadData->load > 0) {
    massData->m_totalLoad = countMassLoadInput();
    massData->m_totalInertial = countInertialMassInput();
  } else {
    massData->m_totalLoad = countMassWithLoad();
    massData->m_totalInertial = countInertialMass();
  }
  movingData->v = 0.0;
  movingData->acc = movingData->acc_start;
  movingData->decc = movingData->decc_start;
}

double TrainSimulation::countMassEmptyCar() {
  massData->m_Me =
      (loadData->n_M1 * massData->m_M1) + (loadData->n_M2 * massData->m_M2);
  massData->m_Te =
      (loadData->n_Tc * massData->m_TC) + (loadData->n_T1 * massData->m_T1) +
      (loadData->n_T2 * massData->m_T2) + (loadData->n_T3 * massData->m_T3);
  return massData->m_Me + massData->m_Te;
}

double TrainSimulation::countMassWithLoad() {
  loadData->m_P_final = loadData->m_P / 1000;
  massData->m_M = massData->m_Me +
                  loadData->n_M1 * (loadData->m_P_final * loadData->n_PM1) +
                  loadData->n_M2 * (loadData->m_P_final * loadData->n_PM2);
  massData->m_T = massData->m_Te +
                  loadData->n_Tc * (loadData->m_P_final * loadData->n_PTc) +
                  loadData->n_T1 * (loadData->m_P_final * loadData->n_PT1) +
                  loadData->n_T2 * (loadData->m_P_final * loadData->n_PT2) +
                  loadData->n_T3 * (loadData->m_P_final * loadData->n_PT3);
  return massData->m_M + massData->m_T;
}

double TrainSimulation::countMassLoadInput() {
  massData->m_totalLoad =
      massData->m_totalEmpty + trainData->n_car * loadData->load;
  return massData->m_totalLoad;
}

double TrainSimulation::countInertialMass() {
  massData->m_Mi = (massData->m_Me * massData->i_M) +
                   (loadData->n_M1 * (loadData->m_P_final * loadData->n_PM1) +
                    loadData->n_M2 * (loadData->m_P_final * loadData->n_PM2));
  massData->m_Ti = (massData->m_Te * massData->i_M) +
                   (loadData->n_Tc * (loadData->m_P_final * loadData->n_PTc) +
                    loadData->n_T1 * (loadData->m_P_final * loadData->n_PT1) +
                    loadData->n_T2 * (loadData->m_P_final * loadData->n_PT2) +
                    loadData->n_T3 * (loadData->m_P_final * loadData->n_PT3));
  return massData->m_Mi + massData->m_Ti;
}

double TrainSimulation::countInertialMassInput() {
  massData->m_totalInertial = (massData->m_Me * massData->i_M) +
                              (massData->m_Te * massData->i_T) +
                              (trainData->n_car * loadData->load);
  return massData->m_totalInertial;
}

double TrainSimulation::calculateResTrain(float m, float startRes) {
  resistanceData->r_train = (m * startRes) / 1000;
  return resistanceData->r_train;
}

double TrainSimulation::calculateResSlope(float m, float slope) {
  resistanceData->r_slope = (m * constantData.g * slope / 1000);
  return resistanceData->r_slope;
}

double TrainSimulation::calculateResRadius(float m, float radius) {
  resistanceData->r_radius = (m * constantData.g * (6.0 / radius)) / 1000;
  return resistanceData->r_radius;
}

double TrainSimulation::calculateStartForce(float acc) {
  resistanceData->f_start =
      massData->m_totalInertial * (acc / constantData.cV) +
      resistanceData->f_resStart;
  return resistanceData->f_start;
}

double TrainSimulation::calculateBrakeForce() {
  resistanceData->f_brake =
      massData->m_totalInertial * (movingData->decc_start / constantData.cV);
  return resistanceData->f_brake;
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
      (((massData->m_M1 + massData->m_M2) * 1000) / trainData->n_axle);
  return trainMotorData->tm_adh;
}

double TrainSimulation::calculateResistanceForcePerMotor(double f_res) {
  trainMotorData->tm_f_res = f_res / (loadData->n_M1 + loadData->n_M2);
  return trainMotorData->tm_f_res;
}

double TrainSimulation::calculatePowerWheel() {
  powerData->p_wheel =
      abs(resistanceData->f_motor) * (movingData->v / constantData.cV);
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
  powerData->p_catenary = powerData->p_vvvfIn * efficiencyData->eff_vvvf;
  return powerData->p_catenary;
}

double TrainSimulation::calculateStartRes() {
  resistanceData->r_train =
      calculateResTrain(massData->m_totalLoad, resistanceData->startRes);
  resistanceData->r_slope =
      calculateResSlope(massData->m_totalInertial, resistanceData->slope);
  resistanceData->r_radius =
      calculateResRadius(massData->m_totalInertial, resistanceData->radius);
  return resistanceData->r_train + resistanceData->r_slope +
         resistanceData->r_radius;
}

double TrainSimulation::calculateRunningRes(float v) {
  resistanceData->r_slope =
      calculateResSlope(massData->m_totalInertial, resistanceData->slope);
  resistanceData->r_radius =
      calculateResRadius(massData->m_totalInertial, resistanceData->radius);
  resistanceData->r_run =
      (1.0 / 1000.0) *
      (((1.65 + (0.0247 * v)) * (massData->m_M * constantData.g)) +
       ((0.78 + (0.0028 * v)) * (massData->m_T * constantData.g)) +
       (constantData.g * (0.028 + 0.0078 * (trainData->n_car - 1)) * (v * v)));
  return resistanceData->r_run + resistanceData->r_slope +
         resistanceData->r_radius;
}

void TrainSimulation::calculatePoweringForce(float acc, float v) {
  if (v <= 0)
    resistanceData->f_start = calculateStartForce(acc);
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
      massData->m_totalInertial * (movingData->decc_start / constantData.cV);
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
    qDebug() << "Invalid input";
  }
}

void TrainSimulation::simulateDynamicTrainMovement() {
  ofstream outFile("F:/matkul/sem_6/AppProject/TrainAppSimulation/formulas/"
                   "fixed_dynamic_simulation.csv",
                   ios::out);
  outFile << "Phase,Iteration,Time,Speed,Acceleration,F Motor,F Res,F Total,F "
             "Motor/TM,F Res/TM,Torque,RPM\n";
  initData();
  qDebug() << "Masses : ";
  qDebug() << "  Empty load:" << massData->m_totalEmpty;
  qDebug() << "  Loaded mass:" << massData->m_totalLoad;
  qDebug() << "  Inertial mass:" << massData->m_totalInertial;
  int i = 0;
  bool isAccelerating = true;
  bool isCoasting = false;
  float time = 0;
  QString phase;
  int coastingCount = 0;
  while (movingData->v >= 0) {
    resistanceData->f_resStart = calculateStartRes();
    resistanceData->f_resRunning = calculateRunningRes(movingData->v);
    if (isAccelerating) {
      if (movingData->v >= movingData->v_limit) {
        isAccelerating = false;
        isCoasting = true;
        phase = "Coasting";
        continue;
      }
      phase = "Accelerating";
      calculatePoweringForce(movingData->acc, movingData->v);
      calculateTotalForce(movingData->v);
      movingData->acc =
          constantData.cV * resistanceData->f_total / massData->m_totalInertial;
      movingData->v += movingData->acc * constantData.dt;
    } else if (isCoasting) {
      if (movingData->v <= (movingData->v_limit - movingData->v_diffCoast)) {
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
      resistanceData->f_motor = 0;
      resistanceData->f_total = -resistanceData->f_resRunning;
      movingData->acc =
          constantData.cV * resistanceData->f_total / massData->m_totalInertial;
      movingData->v += movingData->acc * constantData.dt;
    } else {
      phase = "Braking";
      calculateBrakingForce();
      resistanceData->f_brake = calculateTotalBrakeForce();

      movingData->decc =
          constantData.cV * resistanceData->f_total / massData->m_totalInertial;
      movingData->v += movingData->decc * constantData.dt;
      if (movingData->v <= 0)
        break;
    }
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

    if (i == 0) {
      trainMotorData->tm_adh = calculateAdhesion();
      outFile << "Starting" << "," << i << "," << time << "," << 0 << ","
              << movingData->acc << "," << resistanceData->f_motor << ","
              << (movingData->v > 0 ? resistanceData->f_resRunning
                                    : resistanceData->f_resStart)
              << "," << resistanceData->f_total << "," << trainMotorData->tm_f
              << "," << trainMotorData->tm_f_res << "," << powerData->p_wheel
              << "," << powerData->p_motorOut << "," << powerData->p_motorIn
              << "," << powerData->p_vvvfIn << "," << powerData->p_catenary
              << "\n";
    }
    time += constantData.dt;
    outFile << phase.toStdString() << "," << i + 1 << "," << time << ","
            << movingData->v << ","
            << (isAccelerating || isCoasting ? movingData->acc
                                             : movingData->decc)
            << "," << resistanceData->f_motor << ","
            << (movingData->v > 0 ? resistanceData->f_resRunning
                                  : resistanceData->f_resStart)
            << "," << resistanceData->f_total << "," << trainMotorData->tm_f
            << "," << trainMotorData->tm_f_res << "," << powerData->p_wheel
            << "," << powerData->p_motorOut << "," << powerData->p_motorIn
            << "," << powerData->p_vvvfIn << "," << powerData->p_catenary
            << "\n";
    i++;
  }
  outFile.close();
  emit simulationCompleted();
}

// void TrainSimulation::simulateDynamicTrainMovement() {
//   int i = 0;
//   bool isAccelerating = true;
//   bool isCoasting = false;
//   float time = 0;
//   QString phase = "Starting";
//   int coastingCount = 0;
//   qDebug() << "=== SIMULATION PARAMETERS ===";
//   qDebug() << "Gravitation : " << constantData.g;
//   qDebug() << "Time step : " << constantData.dt;
//   qDebug() << "Conversion to km/h : " << constantData.cV;
//   qDebug() << "conversion to kgf : " << constantData.cF;
//   qDebug() << "Starting Resistance:" << resistanceData->startRes;
//   qDebug() << "Radius:" << resistanceData->radius;
//   qDebug() << "Slope:" << resistanceData->slope;
//   qDebug() << "Acceleration:" << movingData->acc_start;
//   qDebug() << "Deceleration:" << movingData->decc_start;
//   qDebug() << "Speed Limit:" << movingData->v_limit;
//   qDebug() << "Coasting Speed Diff:" << movingData->v_diffCoast;
//   qDebug() << "v_p1:" << movingData->v_p1;
//   qDebug() << "v_p2:" << movingData->v_p2;
//   qDebug() << "v_b1:" << movingData->v_b1;
//   qDebug() << "v_b2:" << movingData->v_b2;
//   qDebug() << "Gear Ratio:" << trainData->gearRatio;
//   qDebug() << "Wheel Diameter:" << trainData->wheel;
//   qDebug() << "Masses : ";
//   qDebug() << "  Empty M1:" << massData->m_M1;
//   qDebug() << "  Empty M2:" << massData->m_M2;
//   qDebug() << "  Empty TC:" << massData->m_TC;
//   qDebug() << "  Empty T1:" << massData->m_T1;
//   qDebug() << "  Empty T2:" << massData->m_T2;
//   qDebug() << "  Empty T3:" << massData->m_T3;
//   qDebug() << "Number of car : ";
//   qDebug() << "  M1:" << loadData->n_M1;
//   qDebug() << "  M2:" << loadData->n_M2;
//   qDebug() << "  TC:" << loadData->n_Tc;
//   qDebug() << "  T1:" << loadData->n_T1;
//   qDebug() << "  T2:" << loadData->n_T2;
//   qDebug() << "  T3:" << loadData->n_T3;
//   qDebug() << "Number of Passenger : ";
//   qDebug() << "  M1:" << loadData->n_PM1;
//   qDebug() << "  M2:" << loadData->n_PM2;
//   qDebug() << "  TC:" << loadData->n_PTc;
//   qDebug() << "  T1:" << loadData->n_PT1;
//   qDebug() << "  T2:" << loadData->n_PT2;
//   qDebug() << "  T3:" << loadData->n_PT3;
//   qDebug() << "Passenger Weight:" << loadData->m_P;
//   qDebug() << "Inertial Coefficient Motor:" << massData->i_M;
//   qDebug() << "Inertial Coefficient Trailer:" << massData->i_T;
//   qDebug() << "Efficiency (gear):" << efficiencyData->eff_gear;
//   qDebug() << "Efficiency (motor):" << efficiencyData->eff_motor;
//   qDebug() << "Efficiency (VVVF):" << efficiencyData->eff_vvvf;
//   qDebug() << "Number of cars:" << trainData->n_car;
//   qDebug() << "Number of axles:" << trainData->n_axle;
//   qDebug() << "Number of traction motors:" << trainData->n_tm;
//   massData->m_totalEmpty = countMassEmptyCar();
//   qDebug() << "Total empty motor mass:" << massData->m_Me;
//   qDebug() << "Total empty trailer mass:" << massData->m_Te;
//   massData->m_totalLoad = countMassWithLoad();
//   massData->m_totalInertial = countInertialMass();
//   qDebug() << "Total empty mass:" << massData->m_totalEmpty;
//   qDebug() << "Total loaded mass:" << massData->m_totalLoad;
//   qDebug() << "Total inertial mass:" << massData->m_totalInertial;
//   qDebug() << "=== SIMULATION STARTING ===";
//   resistanceData->f_resStart = calculateStartRes();
//   resistanceData->f_resRunning = calculateRunningRes(2.0);
//   qDebug() << "Resistance Train : " << resistanceData->r_train;
//   qDebug() << "Resistance Slope : " << resistanceData->r_slope;
//   qDebug() << "Resistance Radius : " << resistanceData->r_radius;
//   qDebug() << "Resistance Start : " << resistanceData->f_resStart;
//   qDebug() << "Resistance Running : " << resistanceData->f_resRunning;
//   calculatePoweringForce(1.0, 0.0);
//   qDebug() << "f_motor : " << resistanceData->f_motor;
//   resistanceData->f_total = calculateTotalForce(0.0);
//   movingData->acc =
//       constantData.cV * resistanceData->f_total /
//       massData->m_totalInertial;
//   movingData->v += movingData->acc * constantData.dt;
//   qDebug() << "Total Force : " << resistanceData->f_total;
//   qDebug() << "Acceleration : " << movingData->acc;
//   qDebug() << "Speed : " << movingData->v;
// }

void TrainSimulation::simulateStaticTrainMovement() {
  ofstream outFile("F:/matkul/sem_6/AppProject/TrainAppSimulation/formulas/"
                   "fixed_static_simulation.csv",
                   ios::out);
  outFile << "Phase,Iteration,Time,Speed,Acceleration,F Motor,F Res,F Total,F "
             "Motor/TM,F Res/TM,Torque,RPM\n";
  initData();
  double v_limit = 130;
  int i = 0;
  bool isAccelerating = true;
  bool isCoasting = false;
  QString phase = "Starting";
  int CoastingCount = 0;
  float time = 0.0;
  while (movingData->v <= v_limit + 1) {
    phase = "Accelerating";
    resistanceData->f_resStart = calculateStartRes();
    resistanceData->f_resRunning = calculateRunningRes(movingData->v);
    calculatePoweringForce(movingData->acc, movingData->v);
    resistanceData->f_total = calculateTotalForce(movingData->v);
    trainMotorData->tm_f_res =
        calculateResistanceForcePerMotor(resistanceData->f_res);
    trainMotorData->tm_f = calculateTractionForce();
    trainMotorData->tm_t = calculateTorque();
    movingData->acc =
        constantData.cV * resistanceData->f_total / massData->m_totalInertial;
    movingData->v += 0.5;
    powerData->p_wheel = calculatePowerWheel();
    powerData->p_motorOut = calculateOutputTractionMotor();
    powerData->p_motorIn = calculateInputTractionMotor();
    powerData->p_vvvfIn = calculatePowerInputOfVvvf();
    powerData->p_catenary = calculatePowerOfCatenary();
    trainMotorData->tm_rpm = calculateRpm();

    if (i == 0) {
      trainMotorData->tm_adh = calculateAdhesion();
      outFile << "Starting" << "," << i << "," << time << "," << 0 << ","
              << movingData->acc << "," << resistanceData->f_motor << ","
              << (movingData->v > 0 ? resistanceData->f_resRunning
                                    : resistanceData->f_resStart)
              << "," << resistanceData->f_total << "," << trainMotorData->tm_f
              << "," << trainMotorData->tm_f_res << "," << powerData->p_wheel
              << "," << powerData->p_motorOut << "," << powerData->p_motorIn
              << "," << powerData->p_vvvfIn << "," << powerData->p_catenary
              << "\n";
    }
    time += constantData.dt;
    outFile << phase.toStdString() << "," << i + 1 << "," << time << ","
            << movingData->v << ","
            << (isAccelerating || isCoasting ? movingData->acc
                                             : movingData->decc)
            << "," << resistanceData->f_motor << ","
            << (movingData->v > 0 ? resistanceData->f_resRunning
                                  : resistanceData->f_resStart)
            << "," << resistanceData->f_total << "," << trainMotorData->tm_f
            << "," << trainMotorData->tm_f_res << "," << powerData->p_wheel
            << "," << powerData->p_motorOut << "," << powerData->p_motorIn
            << "," << powerData->p_vvvfIn << "," << powerData->p_catenary
            << "\n";
    i++;
  }
  emit simulationCompleted();
}

void TrainSimulation::resetSimulation() {
  resistanceData->f_resStart = 0;
  resistanceData->f_resRunning = 0;
  resistanceData->f_res = 0;
  resistanceData->f_motor = 0;
  resistanceData->f_start = 0;
  resistanceData->f_brake = 0;
  resistanceData->f_total = 0;
  resistanceData->f_total = 0;
  trainMotorData->tm_f_res = 0;
  trainMotorData->tm_f = 0;
  trainMotorData->tm_t = 0;
  movingData->acc = movingData->acc_start;
  movingData->v = 0;
  powerData->p_wheel = 0;
  powerData->p_motorOut = 0;
  powerData->p_motorIn = 0;
  powerData->p_vvvfIn = 0;
  powerData->p_catenary = 0;
  trainMotorData->tm_rpm = 0;
}
