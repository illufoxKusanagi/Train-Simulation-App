#include "traction_effort_page.h"

TractionEffortPage::TractionEffortPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)) {
  mainLayout->setAlignment(Qt::AlignCenter);
  mainLayout->setSpacing(16);
  QWidget *runButtonWidget = new QWidget();
  QHBoxLayout *runButtonLayout = new QHBoxLayout(runButtonWidget);
  ButtonAction *runButton = new ButtonAction("Run Simulation", "yes", this);
  ChartWidget *chartWidget = new ChartWidget("Train Speed", "speed", this);
  runButton->setEnabled(true);
  runButtonLayout->addWidget(runButton);
  runButtonLayout->setContentsMargins(0, 0, 0, 0);
  runButtonLayout->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
  runButton->setFixedSize(200, 48);
  mainLayout->addWidget(runButtonWidget);
  mainLayout->addWidget(runButtonWidget);
  mainLayout->addWidget(chartWidget);
  setLayout(mainLayout);
}