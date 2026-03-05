#ifndef FUZZY_VARIABLE_H
#define FUZZY_VARIABLE_H

#include "fuzzy_set.h"
#include <QVector>
#include <memory>

// Holds a named fuzzy variable with a set of membership function terms
// and a current crisp value. Used for both inputs and outputs.
class FuzzyVariable {
public:
  explicit FuzzyVariable(const QString &name, double min = 0.0,
                         double max = 1.0);

  void addTerm(std::shared_ptr<FuzzySet> term);

  // Store a crisp input value (clamped to [min, max])
  void setValue(double val);

  QString name() const;
  double value() const;
  double min() const;
  double max() const;

  const QVector<std::shared_ptr<FuzzySet>> &terms() const;

  // Membership of termName at the currently stored value (for antecedent eval)
  double getMembership(const QString &termName) const;

  // Membership of termName at an arbitrary point x (for centroid integration)
  double getMembershipAt(const QString &termName, double x) const;

  // Name of the term with the highest membership at the current value
  QString getDominantTerm() const;

private:
  QString m_name;
  double m_min;
  double m_max;
  double m_value;
  QVector<std::shared_ptr<FuzzySet>> m_terms;
};

#endif // FUZZY_VARIABLE_H
