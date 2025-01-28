#include "train_parameter_page.h"
#include "../widgets/input_widget.h"

TrainParameterPage::TrainParameterPage(QWidget *parent)
    : QWidget(parent), formLayout(new QGridLayout(this)) {

  setupInputs();

  setLayout(formLayout);
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
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("field", labels[i], unitLabels[i]), this);
    inputWidgets.append(inputWidget);
  }
}
