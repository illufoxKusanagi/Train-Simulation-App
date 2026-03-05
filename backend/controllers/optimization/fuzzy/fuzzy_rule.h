#ifndef FUZZY_RULE_H
#define FUZZY_RULE_H

#include <QMap>
#include <QPair>
#include <QString>

// A single Mamdani IF-THEN rule.
//
// antecedents : variable name → term name  (the IF side, AND-combined)
// consequent  : variable name + term name  (the THEN side)
//
// Example:
//   antecedents["TravelTime"] = "Short"
//   antecedents["MotorPower"] = "Low"
//   consequent = {"FuzzyScore", "Excellent"}
//
// Means: IF TravelTime is Short AND MotorPower is Low THEN FuzzyScore is
// Excellent
struct FuzzyRule {
  QMap<QString, QString> antecedents;
  QPair<QString, QString> consequent;
};

#endif // FUZZY_RULE_H
