#include "running_data_handler.h"

RunningDataHandler::RunningDataHandler(AppContext *context, QObject *parent)
    : QObject(parent), m_movingData(context->movingData.data()),
      m_resistanceData(context->resistanceData.data()),
      m_trainData(context->trainData.data()) {}

void RunningDataHandler::setStartingResistance(double startRes) {
  if (m_resistanceData) {
    m_resistanceData->startRes = startRes;
  }
}

void RunningDataHandler::setWeakeningPoint1(double v_p1) {
  if (m_movingData) {
    m_movingData->v_p1 = v_p1;
  }
}

void RunningDataHandler::setWeakeningPoint2(double v_p2) {
  if (m_movingData) {
    m_movingData->v_p2 = v_p2;
  }
}

void RunningDataHandler::setWeakeningPoint3(double v_b1) {
  if (m_movingData) {
    m_movingData->v_b1 = v_b1;
  }
}

void RunningDataHandler::setWeakeningPoint4(double v_b2) {
  if (m_movingData) {
    m_movingData->v_b2 = v_b2;
  }
}

void RunningDataHandler::setDifferenceCoastingSpeed(double v_diffCoast) {
  if (m_movingData) {
    m_movingData->v_diffCoast = v_diffCoast;
  }
}

void RunningDataHandler::setEmergencyBrakeDeceleration(
    double decc_emergency_si) {
  if (m_movingData) {
    m_movingData->decc_emergency_si = decc_emergency_si;
  }
}

void RunningDataHandler::setAccelerationValue(double acceleration) {
  if (m_movingData) {
    m_movingData->acc_start_si = acceleration;
  }
}

void RunningDataHandler::setDecelerationValue(double decceleration) {
  if (m_movingData) {
    m_movingData->decc_start_si = decceleration;
  }
}
