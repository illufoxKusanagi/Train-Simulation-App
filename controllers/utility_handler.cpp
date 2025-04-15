#include "utility_handler.h"

UtilityHandler::UtilityHandler() {}

void UtilityHander::addSimulationDatas(int i, double time, QString phase) {
  simulationDatas.tractionEfforts.append(resistanceData->f_motor);
  simulationDatas.vvvfPowers.append(powerData->p_vvvfIn);
  simulationDatas.catenaryPowers.append(powerData->p_catenary);
  simulationDatas.vvvfCurrents.append(energyData->curr_vvvf);
  simulationDatas.catenaryCurrents.append(energyData->curr_catenary);
  simulationDatas.distance.append(movingData->x);
  simulationDatas.timeTotal.append(time);
  simulationDatas.distanceTotal.append(movingData->x_total);
  simulationDatas.phase.append(phase);
  simulationDatas.motorForce.append(resistanceData->f_motor);
  simulationDatas.motorResistance.append(i == 0 ? resistanceData->f_resStart
                                                : resistanceData->f_resRunning);
  simulationDatas.totalResistance.append(resistanceData->f_total);
  simulationDatas.tractionForcePerMotor.append(trainMotorData->tm_f);
  simulationDatas.resistancePerMotor.append(trainMotorData->tm_f_res);
  simulationDatas.torque.append(trainMotorData->tm_t);
  simulationDatas.rpm.append(trainMotorData->tm_rpm);
  simulationDatas.powerWheel.append(powerData->p_wheel);
  simulationDatas.powerMotorOut.append(powerData->p_motorOut);
  simulationDatas.powerMotorIn.append(powerData->p_motorIn);
  simulationDatas.energyConsumptions.append(energyData->e_motor);
  simulationDatas.energyPowerings.append(energyData->e_pow);
  simulationDatas.energyRegenerations.append(energyData->e_reg);
  simulationDatas.energyAps.append(energyData->e_aps);
  simulationDatas.energyCatenaries.append(energyData->e_catenary);
}

void UtilityHander::resetSimulation() {
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

void UtilityHander::clearSimulationDatas() {
  simulationDatas.trainSpeeds.clear();
  simulationDatas.trainSpeedsSi.clear();
  simulationDatas.accelerations.clear();
  simulationDatas.accelerationsSi.clear();
  simulationDatas.tractionEfforts.clear();
  simulationDatas.vvvfPowers.clear();
  simulationDatas.catenaryPowers.clear();
  simulationDatas.vvvfCurrents.clear();
  simulationDatas.catenaryCurrents.clear();
  simulationDatas.phase.clear();
  simulationDatas.motorForce.clear();
  simulationDatas.motorResistance.clear();
  simulationDatas.totalResistance.clear();
  simulationDatas.tractionForcePerMotor.clear();
  simulationDatas.resistancePerMotor.clear();
  simulationDatas.torque.clear();
  simulationDatas.rpm.clear();
  simulationDatas.powerWheel.clear();
  simulationDatas.powerMotorOut.clear();
  simulationDatas.powerMotorIn.clear();
  simulationDatas.distance.clear();
  simulationDatas.time.clear();
  simulationDatas.distanceTotal.clear();
  simulationDatas.timeTotal.clear();
  simulationDatas.energyConsumptions.clear();
  simulationDatas.energyPowerings.clear();
  simulationDatas.energyRegenerations.clear();
  simulationDatas.energyAps.clear();
  simulationDatas.energyCatenaries.clear();
  simulationDatas.mileages.clear();
}
