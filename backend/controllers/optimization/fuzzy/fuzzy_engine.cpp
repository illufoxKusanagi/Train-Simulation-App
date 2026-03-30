#include "fuzzy_engine.h"
#include "fuzzy_rule.h"
#include "fuzzy_variable.h"
#include <algorithm>

FuzzyEngine::FuzzyEngine() {}

void FuzzyEngine::addInputVariable(std::shared_ptr<FuzzyVariable> var) {
  m_inputs[var->name()] = var;
}

void FuzzyEngine::addOutputVariable(std::shared_ptr<FuzzyVariable> var) {
  m_outputs[var->name()] = var;
}

void FuzzyEngine::addRule(const FuzzyRule &rule) { m_rules.push_back(rule); }

void FuzzyEngine::clear() {
  m_inputs.clear();
  m_outputs.clear();
  m_rules.clear();
}

void FuzzyEngine::setInputValue(const QString &varName, double value) {
  if (m_inputs.contains(varName)) {
    m_inputs[varName]->setValue(value);
  } else {
  }
}

QString FuzzyEngine::getDominantInputTerm(const QString &varName) const {
  if (m_inputs.contains(varName)) {
    return m_inputs[varName]->getDominantTerm();
  }
  return "";
}

double FuzzyEngine::getOutputValue(const QString &varName) {
  if (!m_outputs.contains(varName)) {
    return 0.0;
  }

  auto outputVar = m_outputs[varName];

  double minVal = outputVar->min();
  double maxVal = outputVar->max();
  int steps = 100;
  double stepSize = (maxVal - minVal) / steps;

  double numerator = 0.0;
  double denominator = 0.0;

  for (int i = 0; i <= steps; ++i) {
    double x = minVal + (i * stepSize);
    double aggregatedMembership = 0.0;

    for (const auto &rule : m_rules) {
      if (rule.consequent.first != varName)
        continue;

      double activation = 1.0;
      for (auto it = rule.antecedents.begin(); it != rule.antecedents.end();
           ++it) {
        QString inputName = it.key();
        QString termName = it.value();

        if (m_inputs.contains(inputName)) {
          double inputMembership = m_inputs[inputName]->getMembership(termName);
          activation = std::min(activation, inputMembership);
        } else {
          activation = 0.0;
        }
      }

      if (activation > 0.0) {
        double termMembership =
            outputVar->getMembershipAt(rule.consequent.second, x);
        double clippedMembership = std::min(activation, termMembership);
        aggregatedMembership =
            std::max(aggregatedMembership, clippedMembership);
      }
    }

    numerator += x * aggregatedMembership;
    denominator += aggregatedMembership;
  }

  if (denominator == 0.0)
    return 0.0;
  return numerator / denominator;
}
