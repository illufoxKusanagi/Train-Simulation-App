#ifndef INPUTTYPE_H
#define INPUTTYPE_H

#include <QString>

class InputType {
public:
  QString type;
  QString label;
  QString unit;

  InputType(const QString &inputType, const QString &inputLabel,
            const QString &inputUnit = "")
      : type(inputType), label(inputLabel), unit(inputUnit) {}
};

#endif // INPUTTYPE_H
