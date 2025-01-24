#include "train_parameter_page.h"

TrainParameterWidget::TrainParameterWidget(QWidget *parent)
    : QWidget(parent), formLayout(new QGridLayout(this)) {
  labels = {"Inertial Coefficient Trailer",
            "Number of Axle",
            "Inertial Coefficient Motor",
            "Number of Traction Motor",
            "Wheel Diameter (mm)",
            "Passenger Weight (kg)",
            "Gear Ratio",
            "Load per Car (ton)"};

  unitLabels = {"", "", "", "", "mm", "kg", "", "ton"};

  for (int i = 0; i < labels.size(); ++i) {
    QLabel *label = new QLabel(labels[i], this);
    QLineEdit *input = new QLineEdit(this);
    QLabel *unitLabel = new QLabel(unitLabels[i], this);

    formLayout->addWidget(label, i / 2, (i % 2) * 3);
    formLayout->addWidget(input, i / 2, (i % 2) * 3 + 1);
    formLayout->addWidget(unitLabel, i / 2, (i % 2) * 3 + 2);
  }

  setLayout(formLayout);
}
