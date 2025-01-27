#include "constant_values_page.h"
#include <QGroupBox>

ConstantValuesPage::ConstantValuesPage(QWidget *parent) : QWidget(parent) {
  QBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  QGroupBox *formLayout = new QGroupBox("Constant Values");
  QVBoxLayout *groupBoxLayout = new QVBoxLayout(formLayout);
  QStringList labels = {"Gravitation", "m/s to km/h"};
  QStringList unitLabels = {"m/s^2", "km/h"};
  InputType gravitationInputType("field", "Gravitation", "m/s^2");
  InputType converterInputType("field", "m/s to km/h", "km/h");
  InputWidget *gravitationInput = new InputWidget(gravitationInputType, this);
  InputWidget *converterInput = new InputWidget(converterInputType, this);
  groupBoxLayout->addWidget(gravitationInput);
  groupBoxLayout->addWidget(converterInput);
  formLayout->setLayout(groupBoxLayout);
  mainLayout->addWidget(formLayout);
  setFixedSize(900, 500);
  setStyleSheet("QGroupBox { border: 1px solid gray; border-radius: 5px; "
                "margin-top: 1.5em; max-width: 300; max-height: 300; }");
}
