#ifndef FUZZY_SET_H
#define FUZZY_SET_H

#include <QString>

// Abstract base class for all fuzzy membership functions.
// Derived classes implement membership(value) to return a degree in [0.0, 1.0].
class FuzzySet {
public:
  explicit FuzzySet(const QString &name);
  virtual ~FuzzySet() = default;

  QString name() const;
  virtual double membership(double value) const = 0;

protected:
  QString m_name;
};

#endif // FUZZY_SET_H
