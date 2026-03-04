#ifndef FUZZY_ENGINE_H
#define FUZZY_ENGINE_H

#include "fuzzy_rule.h"
#include "fuzzy_variable.h"
#include <QMap>
#include <QVector>
#include <memory>

// Mamdani fuzzy inference engine.
// Supports multiple input variables, one output variable, and any number of
// IF-THEN rules. Defuzzification uses centroid (centre of gravity, 100 steps).
class FuzzyEngine {
public:
  FuzzyEngine();
  ~FuzzyEngine() = default;

  void addInputVariable(std::shared_ptr<FuzzyVariable> var);
  void addOutputVariable(std::shared_ptr<FuzzyVariable> var);
  void addRule(const FuzzyRule &rule);

  // Clear all variables and rules — call before reconfiguring between runs
  void clear();

  // Set a crisp input value on a named input variable
  void setInputValue(const QString &varName, double value);

  // Run inference and defuzzify; returns centroid of the output variable
  double getOutputValue(const QString &varName);

  // Returns the dominant (highest-membership) term name for a named input
  QString getDominantInputTerm(const QString &varName) const;

private:
  QMap<QString, std::shared_ptr<FuzzyVariable>> m_inputs;
  QMap<QString, std::shared_ptr<FuzzyVariable>> m_outputs;
  QVector<FuzzyRule> m_rules;
};

#endif // FUZZY_ENGINE_H
