#include "running_parameter_page.h"

RunningParameterPage::RunningParameterPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_formLayout(new QWidget(this)),
      m_inputsLayout(new QGridLayout(m_formLayout)) {
  mainLayout->setAlignment(Qt::AlignCenter);
  m_formLayout->setContentsMargins(16, 16, 16, 16);
  m_inputsLayout->setHorizontalSpacing(80);
  m_inputsLayout->setVerticalSpacing(24);

  createInputs();
  mainLayout->addWidget(m_formLayout);
  setLayout(mainLayout);
}

void RunningParameterPage::createInputs() {
  QStringList labels = {
      "Starting Resistance",
      "Weakening Point 1 (Powering)",
      "Acceleration",
      "Weakening Point 2 (Powering)",
      "Deceleration",
      "Weakening Point 3 (Braking)",
      "Difference Coasting Speed",
      "Weakening Point 4 (Braking)",
  };
  QStringList unitLabels = {"",      "km/h", "m/s^2", "km/h",
                            "m/s^2", "km/h", "km/h",  "km/h"};
  QList<double> values = {39.2, 35, 1, 65, 1, 1, 5, 70};

  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("field", labels[i], unitLabels[i]), this);
    inputWidget->setValue(values[i]);
    m_inputsLayout->addWidget(inputWidget, i / 2, i % 2);
    m_inputWidgets[labels[i]] = inputWidget;
  }
}