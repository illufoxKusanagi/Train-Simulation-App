#include "mainwindow.h"

MainWindow::MainWindow(AppContext &context, QWidget *parent)
    : QMainWindow(parent), context(context) {
  QWidget *centralWidget = new QWidget(this);
  QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  m_trainSimulation = new TrainSimulation(context, this);
  leftPanel = new LeftPanel(this, m_trainSimulation);
  stackedWidget = new QStackedWidget(this);
  connect(leftPanel, &LeftPanel::navigateToPage, this,
          &MainWindow::navigateToPage);
  setupPages();
  mainLayout->addWidget(leftPanel);
  mainLayout->addWidget(stackedWidget);
  centralWidget->setLayout(mainLayout);
  setCentralWidget(centralWidget);
  navigateToPage(0);
}

void MainWindow::setupPages() {
  auto setupFixedSizePage = [this](QWidget *page) {
    page->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    stackedWidget->addWidget(page);
  };
  ConstantValuesPage *constantValuesPage = new ConstantValuesPage(this);
  setupFixedSizePage(constantValuesPage);

  TrainParameterPage *trainParameterPage =
      new TrainParameterPage(context, m_trainSimulation, this);
  setupFixedSizePage(trainParameterPage);

  RunningParameterPage *runningParameterPage =
      new RunningParameterPage(context, this);
  setupFixedSizePage(runningParameterPage);

  TrackParameterPage *trackParameterPage =
      new TrackParameterPage(context, this);
  setupFixedSizePage(trackParameterPage);

  ElectricalParameterPage *electricalParameterPage =
      new ElectricalParameterPage(context, this);
  setupFixedSizePage(electricalParameterPage);

  TrainSpeedPage *trainSpeedPage = new TrainSpeedPage(this, m_trainSimulation);
  setupFixedSizePage(trainSpeedPage);

  TractionEffortPage *tractionEffortPage =
      new TractionEffortPage(this, m_trainSimulation);
  setupFixedSizePage(tractionEffortPage);

  TrainPowerPage *trainPowerPage = new TrainPowerPage(this, m_trainSimulation);
  setupFixedSizePage(trainPowerPage);

  TrainTrackPage *trainTrackPage = new TrainTrackPage(this, m_trainSimulation);
  setupFixedSizePage(trainTrackPage);

  TrainConsumptionPage *trainConsumptionPage =
      new TrainConsumptionPage(this, m_trainSimulation);
  setupFixedSizePage(trainConsumptionPage);

  connect(trainParameterPage, &TrainParameterPage::awDataChanged, this,
          [this, trainParameterPage, runningParameterPage]() {
            double awIndex = trainParameterPage->getAwData();
            runningParameterPage->onAwChanged(awIndex);
          });
}

void MainWindow::navigateToPage(int pageIndex) {
  if (pageIndex >= 0 && pageIndex < stackedWidget->count()) {
    stackedWidget->setCurrentIndex(pageIndex);
  }
}
