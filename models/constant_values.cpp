#include "constant_values.h"

ConstantValues::ConstantValues(QObject *parent) : QObject(parent) {
  m_gravitation = 9.8;
  m_speedConverter = 3.6;
  m_forceConverter = 101.972;
}

double ConstantValues::getGravitation() const { return m_gravitation; }
double ConstantValues::getSpeedConverter() const { return m_speedConverter; }
double ConstantValues::getForceConverter() const { return m_forceConverter; }
