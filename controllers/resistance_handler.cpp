#include "resistance_handler.h"

ResistanceHandler::ResistanceHandler(ResistanceData &resistanceData,
                                     ConstantData &constantData)
    : resistanceData(&resistanceData), constantData(&constantData) {}

double ResistanceHandler::calculateResTrain(float m, float startRes) {
  resistanceData->r_train = (m * startRes) / 1000;
  return resistanceData->r_train;
}

double ResistanceHandler::calculateResSlope(float m, float slope) {
  resistanceData->r_slope = (m * constantData->g * slope) / 1000;
  return resistanceData->r_slope;
}

double ResistanceHandler::calculateResRadius(float m, float radius) {
  resistanceData->r_radius = (m * constantData->g * (6.0 / radius)) / 1000;
  return resistanceData->r_radius;
}
