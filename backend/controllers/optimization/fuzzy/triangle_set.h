#ifndef TRIANGLE_SET_H
#define TRIANGLE_SET_H

#include "fuzzy_set.h"

// Triangular membership function.
//
//      1 |      /\
//        |     /  \
//      0 |____/    \____
//           min  peak  max
//
// membership = 0 outside [min, max], rises linearly to 1 at peak.
class TriangleSet : public FuzzySet {
public:
  TriangleSet(const QString &name, double min, double peak, double max);

  double membership(double value) const override;

private:
  double m_min;
  double m_peak;
  double m_max;
};

#endif // TRIANGLE_SET_H
