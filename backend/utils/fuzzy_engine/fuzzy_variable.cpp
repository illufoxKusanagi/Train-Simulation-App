#include "fuzzy_variable.h"

FuzzyVariable::FuzzyVariable(const QString &name, double min, double max)
    : m_name(name), m_min(min), m_max(max), m_value(0.0) {}

void FuzzyVariable::addTerm(std::shared_ptr<FuzzySet> term) {
  m_terms.push_back(term);
}

void FuzzyVariable::setValue(double val) {
  if (val < m_min)
    val = m_min;
  if (val > m_max)
    val = m_max;
  m_value = val;
}

QString FuzzyVariable::name() const { return m_name; }
double FuzzyVariable::value() const { return m_value; }
double FuzzyVariable::min() const { return m_min; }
double FuzzyVariable::max() const { return m_max; }

const QVector<std::shared_ptr<FuzzySet>> &FuzzyVariable::terms() const {
  return m_terms;
}

double FuzzyVariable::getMembership(const QString &termName) const {
  for (const auto &term : m_terms) {
    if (term->name() == termName)
      return term->membership(m_value);
  }
  return 0.0;
}

double FuzzyVariable::getMembershipAt(const QString &termName, double x) const {
  for (const auto &term : m_terms) {
    if (term->name() == termName)
      return term->membership(x);
  }
  return 0.0;
}

QString FuzzyVariable::getDominantTerm() const {
  double maxM = -1.0;
  QString bestTerm;
  for (const auto &term : m_terms) {
    double m = term->membership(m_value);
    if (m > maxM) {
      maxM = m;
      bestTerm = term->name();
    }
  }
  return bestTerm;
}
