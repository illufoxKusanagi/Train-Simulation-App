#include "train_parameter_page.h"
#include "../widgets/input_widget.h"

TrainParameterPage::TrainParameterPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)) {
  mainLayout->setAlignment(Qt::AlignCenter);
  setLayout(mainLayout);
  setupInputs();
}

void TrainParameterPage::setupInputs() {
  QStringList labels = {"Inertial Coefficient Trailer",
                        "Number of Axle",
                        "Inertial Coefficient Motor",
                        "Number of Traction Motor",
                        "Wheel Diameter (mm)",
                        "Passenger Weight (kg)",
                        "Gear Ratio",
                        "Load per Car (ton)"};

  QStringList unitLabels = {"", "", "", "", "mm", "kg", "", "ton"};
  QWidget *formContainer = new QWidget(this);
  QGridLayout *formLayout = new QGridLayout(formContainer);
  formLayout->setContentsMargins(16, 16, 16, 16);
  formLayout->setHorizontalSpacing(64);
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("field", labels[i], unitLabels[i]), this);
    inputWidgets.append(inputWidget);
    formLayout->addWidget(inputWidget, i / 2, i % 2);
  }
  mainLayout->addWidget(formContainer);
}
