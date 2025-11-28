#ifndef FUZZY_ENGINE_H
#define FUZZY_ENGINE_H

#include <QMap>
#include <QString>
#include <QVector>
#include <cmath>
#include <functional>
#include <memory>

// Forward declarations
class FuzzyEngine;

// ==========================================
// Fuzzy Set (Membership Function)
// ==========================================
class FuzzySet {
public:
  explicit FuzzySet(const QString &name) : m_name(name) {}
  virtual ~FuzzySet() = default;

  QString name() const { return m_name; }
  virtual double membership(double value) const = 0;

protected:
  QString m_name;
};

class TriangleSet : public FuzzySet {
public:
  TriangleSet(const QString &name, double min, double peak, double max)
      : FuzzySet(name), m_min(min), m_peak(peak), m_max(max) {}

  double membership(double value) const override {
    if (value <= m_min || value >= m_max)
      return 0.0;
    if (value == m_peak)
      return 1.0;
    if (value < m_peak)
      return (value - m_min) / (m_peak - m_min);
    return (m_max - value) / (m_max - m_peak);
  }

private:
  double m_min, m_peak, m_max;
};

class TrapezoidSet : public FuzzySet {
public:
  TrapezoidSet(const QString &name, double min, double peak1, double peak2,
               double max)
      : FuzzySet(name), m_min(min), m_peak1(peak1), m_peak2(peak2), m_max(max) {
  }

  double membership(double value) const override {
    if (value <= m_min || value >= m_max)
      return 0.0;
    if (value >= m_peak1 && value <= m_peak2)
      return 1.0;
    if (value < m_peak1)
      return (value - m_min) / (m_peak1 - m_min);
    return (m_max - value) / (m_max - m_peak2);
  }

private:
  double m_min, m_peak1, m_peak2, m_max;
};

// ==========================================
// Fuzzy Variable (Input/Output)
// ==========================================
class FuzzyVariable {
public:
  explicit FuzzyVariable(const QString &name, double min = 0.0,
                         double max = 1.0)
      : m_name(name), m_min(min), m_max(max), m_value(0.0) {}

  void addTerm(std::shared_ptr<FuzzySet> term) { m_terms.push_back(term); }

  void setValue(double val) {
    if (val < m_min)
      val = m_min;
    if (val > m_max)
      val = m_max;
    m_value = val;
  }

  double value() const { return m_value; }
  QString name() const { return m_name; }
  double min() const { return m_min; }
  double max() const { return m_max; }

  // Returns the membership value of a specific term for the current variable
  // value
  double getMembership(const QString &termName) const {
    for (const auto &term : m_terms) {
      if (term->name() == termName) {
        return term->membership(m_value);
      }
    }
    return 0.0;
  }

  // Returns the term with the highest membership for the current value
  QString getDominantTerm() const {
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

  const QVector<std::shared_ptr<FuzzySet>> &terms() const { return m_terms; }

private:
  QString m_name;
  double m_min, m_max;
  double m_value; // Current crisp value
  QVector<std::shared_ptr<FuzzySet>> m_terms;
};

// ==========================================
// Fuzzy Rule
// ==========================================
struct FuzzyRule {
  // Antecedents: Map of VariableName -> TermName (e.g., "Acceleration" ->
  // "Low")
  QMap<QString, QString> antecedents;

  // Consequent: VariableName -> TermName (e.g., "Suitability" -> "NotSuitable")
  QPair<QString, QString> consequent;
};

// ==========================================
// Fuzzy Engine
// ==========================================
class FuzzyEngine {
public:
  FuzzyEngine();
  ~FuzzyEngine() = default;

  // Setup
  void addInputVariable(std::shared_ptr<FuzzyVariable> var);
  void addOutputVariable(std::shared_ptr<FuzzyVariable> var);
  void addRule(const FuzzyRule &rule);

  // Operation
  void setInputValue(const QString &varName, double value);
  double getOutputValue(const QString &varName); // Defuzzifies using Centroid

  // Diagnostics
  QString getDominantInputTerm(const QString &varName) const;

private:
  QMap<QString, std::shared_ptr<FuzzyVariable>> m_inputs;
  QMap<QString, std::shared_ptr<FuzzyVariable>> m_outputs;
  QVector<FuzzyRule> m_rules;
};

#endif // FUZZY_ENGINE_H
