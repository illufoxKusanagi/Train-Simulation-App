#include "electrical_parameter_page.h"

ElectricalParameterPage::ElectricalParameterPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      formContainer(new QWidget(this)),
      formLayout(new QGridLayout(formContainer)) {
  mainLayout->setContentsMargins(16, 16, 16, 16);
  mainLayout->setAlignment(Qt::AlignCenter);
  formLayout->setHorizontalSpacing(80);
  formLayout->setVerticalSpacing(32);
  createInputs();
  mainLayout->addWidget(formContainer);
  setLayout(mainLayout);
}

void ElectricalParameterPage::createInputs() {
  QStringList labels = {"VVVF Voltage",
                        "Power Factor of Traction Motor",
                        "Motor Voltage",
                        "Efficiency of Gearbox",
                        "Efficiancy of VVVF",
                        "Auxiliary Power",
                        "Efficiency of Traction Motor"};
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("upload", labels[i], ""), this);
    formLayout->addWidget(inputWidget, i / 2, i % 2);
  }
}