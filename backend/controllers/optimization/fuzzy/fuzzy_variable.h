#ifndef FUZZY_VARIABLE_H
#define FUZZY_VARIABLE_H

#include "fuzzy_set.h"
#include <QVector>
#include <memory>

class FuzzyVariable {
public:
  explicit FuzzyVariable(const QString &name, double min = 0.0,
                         double max = 1.0);

  void addTerm(std::shared_ptr<FuzzySet> term);
  void setValue(double val);

  QString name() const;
  double value() const;
  double min() const;
  double max() const;

  const QVector<std::shared_ptr<FuzzySet>> &terms() const;

  double getMembership(const QString &termName) const;
  double getMembershipAt(const QString &termName, double x) const;

  QString getDominantTerm() const;

private:
  QString m_name;
  double m_min;
  double m_max;
  double m_value;
  QVector<std::shared_ptr<FuzzySet>> m_terms;
};

#endif // FUZZY_VARIABLE_H
