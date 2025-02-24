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
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("upload", labels[i], ""), this);
    m_formLayout->addWidget(inputWidget, i / 2, i % 2);
  }
}