#include "running_parameter_page.h"

RunningParameterPage::RunningParameterPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)), formLayout(nullptr),
      inputsLayout(nullptr) {
  mainLayout->setAlignment(Qt::AlignCenter);
  formLayout = new QWidget();
  formLayout->setContentsMargins(16, 16, 16, 16);
  inputsLayout = new QGridLayout(formLayout);
  inputsLayout->setHorizontalSpacing(80);
  inputsLayout->setVerticalSpacing(24);

  createInputs();
  mainLayout->addWidget(formLayout);
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

  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("field", labels[i], unitLabels[i]), this);
    inputsLayout->addWidget(inputWidget, i / 2, i % 2);
  }
}