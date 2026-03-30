#ifndef TRAPEZOID_SET_H
#define TRAPEZOID_SET_H

#include "fuzzy_set.h"

class TrapezoidSet : public FuzzySet {
public:
  TrapezoidSet(const QString &name, double min, double peak1, double peak2,
               double max);

  double membership(double value) const override;

private:
  double m_min;
  double m_peak1;
  double m_peak2;
  double m_max;
};

#endif // TRAPEZOID_SET_H
