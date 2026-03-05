#include "triangle_set.h"

TriangleSet::TriangleSet(const QString &name, double min, double peak,
                         double max)
    : FuzzySet(name), m_min(min), m_peak(peak), m_max(max) {}

double TriangleSet::membership(double value) const {
  if (value <= m_min || value >= m_max)
    return 0.0;
  if (value == m_peak)
    return 1.0;
  if (value < m_peak)
    return (value - m_min) / (m_peak - m_min);
  return (m_max - value) / (m_max - m_peak);
}
