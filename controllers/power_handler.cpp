#include "power_handler.h"

PowerHandler::PowerHandler(PowerData &powerData, ResistanceData &resistanceData,
                           MovingData &movingData,
                           EfficiencyData &efficiencyData,
                           EnergyData &energyData, ConstantData &constantData)
    : powerData(&powerData), resistanceData(&resistanceData),
      movingData(&movingData), efficiencyData(&efficiencyData),
      energyData(&energyData), constantData(&constantData)

{}

double PowerHandler::calculatePowerWheel() {
  powerData->p_wheel =
      resistanceData->f_motor * (movingData->v / constantData->cV);
  return powerData->p_wheel;
}

double PowerHandler::calculateOutputTractionMotor() {
  powerData->p_motorOut = powerData->p_wheel / efficiencyData->eff_gear;
  return powerData->p_motorOut;
}

double PowerHandler::calculateInputTractionMotor() {
  powerData->p_motorIn = powerData->p_motorOut / efficiencyData->eff_motor;
  return powerData->p_motorIn;
}

double PowerHandler::calculatePowerInputOfVvvf() {
  powerData->p_vvvfIn = powerData->p_motorIn / efficiencyData->eff_vvvf;
  return powerData->p_vvvfIn;
}

double PowerHandler::calculatePowerOfCatenary() {
  powerData->p_catenary = powerData->p_vvvfIn + powerData->p_aps;
  return powerData->p_catenary;
}
