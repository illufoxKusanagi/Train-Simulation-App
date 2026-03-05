#ifndef TRAPEZOID_SET_H
#define TRAPEZOID_SET_H

#include "fuzzy_set.h"

// Trapezoidal membership function.
//
//      1 |    /‾‾‾‾‾\
//        |   /       \
//      0 |__/         \__
//          min p1   p2  max
//
// membership = 0 outside [min, max], rises to 1 at peak1, stays 1 until peak2,
// then falls back to 0 at max. Use this for boundary terms (Short, Long, Low,
// High).
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
