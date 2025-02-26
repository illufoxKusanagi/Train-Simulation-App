#include "electrical_parameter_page.h"

ElectricalParameterPage::ElectricalParameterPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_formContainer(new QWidget(this)),
      m_formLayout(new QGridLayout(m_formContainer)) {
  mainLayout->setContentsMargins(16, 16, 16, 16);
  mainLayout->setAlignment(Qt::AlignCenter);
  m_formLayout->setHorizontalSpacing(80);
  m_formLayout->setVerticalSpacing(32);
  createInputs();
  mainLayout->addWidget(m_formContainer);
  setLayout(mainLayout);
}

void ElectricalParameterPage::createInputs() {
  QStringList labels = {"VVVF Voltage",
                        "Power Factor of Traction Motor",
                        "Motor Voltage",
                        "Efficiency of Gearbox",
                        "Efficiency of VVVF",
                        "Auxiliary Power",
                        "Efficiency of Traction Motor"};
  QStringList units = {"V", "%", "V", "%", "%", "kW", "%"};
  QList<double> values = {0, 0, 0, 0.98, 0.96, 30, 0.89};
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("field", labels[i], ""), this);
    inputWidget->setValue(values[i]);
    m_inputWidgets[labels[i]] = inputWidget;
    m_formLayout->addWidget(inputWidget, i / 2, i % 2);
  }
}