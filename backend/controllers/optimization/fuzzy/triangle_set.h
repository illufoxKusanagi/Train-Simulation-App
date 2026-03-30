#ifndef TRIANGLE_SET_H
#define TRIANGLE_SET_H

#include "fuzzy_set.h"

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
