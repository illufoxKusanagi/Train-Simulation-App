#include "train_speed_page.h"
#include "../widgets/chart_widget.h"

TrainSpeedPage::TrainSpeedPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)) {
  mainLayout->setAlignment(Qt::AlignCenter);
  setLayout(mainLayout);

  QWidget *runButtonWidget = new QWidget();
  QHBoxLayout *runButtonLayout = new QHBoxLayout(runButtonWidget);
  ButtonAction *runButton = new ButtonAction("Run Simulation", "yes", this);
  ChartWidget *chartWidget = new ChartWidget("Train Speed", "speed", this);
  runButton->setEnabled(true);
  runButtonLayout->addWidget(runButton);
  runButtonLayout->setAlignment(Qt::AlignBottom | Qt::AlignLeft);
  runButton->setFixedSize(200, 48);
  runButtonWidget->setStyleSheet("QWidget#runButtonContainer {"
                                 "   background-color: red;"
                                 "   position: absolute;"
                                 "   margin-top: 20px;"
                                 "   margin-right: 20px;"
                                 "}");
  mainLayout->addWidget(runButtonWidget, 0, Qt::AlignTop | Qt::AlignRight);
  mainLayout->addWidget(runButtonWidget);
  mainLayout->addWidget(chartWidget);
}