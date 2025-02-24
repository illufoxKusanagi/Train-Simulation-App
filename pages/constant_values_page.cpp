#include "constant_values_page.h"

ConstantValuesPage::ConstantValuesPage(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setAlignment(Qt::AlignCenter);
  QGroupBox *formLayout = new QGroupBox("Constant Values");
  formLayout->setContentsMargins(16, 16, 16, 16);
  QVBoxLayout *groupBoxLayout = new QVBoxLayout(formLayout);
  groupBoxLayout->setContentsMargins(16, 16, 16, 16);

  QStringList labels = {"Gravitation", "m/s to km/h"};
  QStringList unitLabels = {"m/s^2", "km/h"};
  InputType gravitationInputType("field", "Gravitation", "m/s^2", 9.8, true);
  InputType speedConverterInputType("field", "m/s to km/h", "", 3.6, true);
  InputType forceConverterInputType("field", "kW to kgf", "", 101.972, true);
  InputWidget *gravitationInput = new InputWidget(gravitationInputType, this);
  InputWidget *speedConverterInput =
      new InputWidget(speedConverterInputType, this);
  InputWidget *forceConverterInput =
      new InputWidget(forceConverterInputType, this);
  groupBoxLayout->addWidget(gravitationInput);
  groupBoxLayout->addWidget(speedConverterInput);
  groupBoxLayout->addWidget(forceConverterInput);

  formLayout->setLayout(groupBoxLayout);
  mainLayout->addWidget(formLayout);
  setStyleSheet("QGroupBox { "
                "border: 1px solid" +
                Colors::Grey300.name() +
                ";"
                "padding: 16px; border-radius: 12px; " +
                TextStyle::BodyMediumRegular() +
                "margin-top: 20px;"
                "min-width: 200px;"
                "}"
                "    QGroupBox::title {"
                "subcontrol-origin: border;"
                "subcontrol-position: top left;"
                "background-color: white;"
                "padding: 0 5px;"
                "position: relative;"
                "left: 20px;"
                "top: -8px;"
                "}");
}
