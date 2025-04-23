#include "power_handler.h"

PowerHandler::PowerHandler(AppContext &context)
    : powerData(context.powerData.data()),
      resistanceData(context.resistanceData.data()),
      movingData(context.movingData.data()),
      efficiencyData(context.efficiencyData.data()),
      energyData(context.energyData.data()),
      constantData(context.constantData.data()) {}

double PowerHandler::calculatePowerWheel() {
  powerData->p_wheel =
      resistanceData->f_motor * (movingData->v / constantData->cV);
  return powerData->p_wheel;
}

double PowerHandler::calculateOutputTractionMotor() {
  powerData->p_motorOut = powerData->p_wheel / efficiencyData->stat_eff_gear;
  return powerData->p_motorOut;
}

double PowerHandler::calculateInputTractionMotor() {
  powerData->p_motorIn = powerData->p_motorOut / efficiencyData->stat_eff_motor;
  return powerData->p_motorIn;
}

double PowerHandler::calculatePowerInputOfVvvf() {
  powerData->p_vvvfIn = powerData->p_motorIn / efficiencyData->stat_eff_vvvf;
  return powerData->p_vvvfIn;
}

double PowerHandler::calculatePowerOfCatenary() {
  powerData->p_catenary = powerData->p_vvvfIn + powerData->p_aps;
  return powerData->p_catenary;
}
