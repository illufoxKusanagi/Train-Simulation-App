#include "current_handler.h"

CurrentHandler::CurrentHandler(AppContext &context)
    : powerData(context.powerData.data()),
      energyData(context.energyData.data()) {}

double CurrentHandler::calculateVvvfCurrent() {
  energyData->curr_vvvf = 1000 * (powerData->p_vvvfIn / energyData->vol_line);
  return energyData->curr_vvvf;
}

double CurrentHandler::calculateCatenaryCurrent() {
  energyData->curr_catenary =
      1000 * (powerData->p_catenary / energyData->vol_line);
  return energyData->curr_catenary;
}