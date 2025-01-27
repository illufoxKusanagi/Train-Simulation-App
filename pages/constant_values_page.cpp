#include "constant_values_page.h"

ConstantValuesPage::ConstantValuesPage(QWidget *parent)
    : QWidget(parent), formLayout(new QGridLayout(this)) {
  QStringList labels = {"Gravitation", "m/s to km/h"};
  QStringList unitLabels = {"m/s^2", "km/h"};
  InputType gravitationInputType("field", "Gravitation", "m/s^2");
  InputType converterInputType("field", "m/s to km/h", "km/h");
  InputWidget *gravitationInput = new InputWidget(gravitationInputType, this);
  InputWidget *converterInput = new InputWidget(converterInputType, this);
  formLayout->addWidget(gravitationInput);
  formLayout->addWidget(converterInput);
}