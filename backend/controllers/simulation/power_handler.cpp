#include "power_handler.h"

PowerHandler::PowerHandler(AppContext &context)
    : powerData(context.powerData.data()),
      resistanceData(context.resistanceData.data()),
      movingData(context.movingData.data()),
      efficiencyData(context.efficiencyData.data()),
      energyData(context.energyData.data()),
      constantData(context.constantData.data()),
      trainData(context.trainData.data()) {}

double PowerHandler::calculatePowerWheel() {
  powerData->p_wheel =
      resistanceData->f_motor * (movingData->v / constantData->cV);
  return powerData->p_wheel;
}

double PowerHandler::calculateOutputTractionMotor(double eff_gear) {
  powerData->p_motorOut = powerData->p_wheel / eff_gear;
  return powerData->p_motorOut;
}

double PowerHandler::calculateOutputTractionMotorPerMotor() {
  if (trainData->n_tm > 0) {
    powerData->p_motorOutPerMotor = powerData->p_motorOut / trainData->n_tm;
  } else {
    powerData->p_motorOutPerMotor = 0;
  }
  return powerData->p_motorOutPerMotor;
}

double PowerHandler::calculateInputTractionMotor(double eff_motor) {
  powerData->p_motorIn = powerData->p_motorOut / eff_motor;
  return powerData->p_motorIn;
}

double PowerHandler::calculatePowerInputOfVvvf(double eff_vvvf) {
  powerData->p_vvvfIn = powerData->p_motorIn / eff_vvvf;
  return powerData->p_vvvfIn;
}

double PowerHandler::calculatePowerOfCatenary() {
  powerData->p_catenary = powerData->p_vvvfIn + powerData->p_aps;
  return powerData->p_catenary;
}
