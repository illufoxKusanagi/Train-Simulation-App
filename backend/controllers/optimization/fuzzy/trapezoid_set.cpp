#include "trapezoid_set.h"

TrapezoidSet::TrapezoidSet(const QString &name, double min, double peak1,
                           double peak2, double max)
    : FuzzySet(name), m_min(min), m_peak1(peak1), m_peak2(peak2), m_max(max) {}

double TrapezoidSet::membership(double value) const {
  if (value <= m_min || value >= m_max)
    return 0.0;
  if (value >= m_peak1 && value <= m_peak2)
    return 1.0;
  if (value < m_peak1)
    return (value - m_min) / (m_peak1 - m_min);
  return (m_max - value) / (m_max - m_peak2);
}
