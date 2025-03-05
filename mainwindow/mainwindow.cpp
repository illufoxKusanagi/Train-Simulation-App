#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  QWidget *centralWidget = new QWidget(this);
  QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);
  m_trainSimulation =
      new TrainSimulation(this, &m_trainData, &m_massData, &m_loadData,
                          &m_resistanceData, &m_movingData, &m_trainMotorData,
                          &m_efficiencyData, &m_powerData, &m_energyData);
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
  const int PAGE_WIDTH = 960;
  const int PAGE_HEIGHT = 732;
  auto setupFixedSizePage = [this, PAGE_WIDTH, PAGE_HEIGHT](QWidget *page) {
    page->setFixedSize(PAGE_WIDTH, PAGE_HEIGHT);
    page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    stackedWidget->addWidget(page);
  };
  ConstantValuesPage *constantValuesPage = new ConstantValuesPage(this);
  setupFixedSizePage(constantValuesPage);

  TrainParameterPage *trainParameterPage = new TrainParameterPage(
      this, &m_trainData, &m_massData, &m_loadData, m_trainSimulation);
  setupFixedSizePage(trainParameterPage);

  RunningParameterPage *runningParameterPage =
      new RunningParameterPage(this, &m_movingData, &m_resistanceData);
  setupFixedSizePage(runningParameterPage);

  TrackParameterPage *trackParameterPage =
      new TrackParameterPage(this, &m_movingData, &m_resistanceData);
  setupFixedSizePage(trackParameterPage);

  ElectricalParameterPage *electricalParameterPage =
      new ElectricalParameterPage(this, &m_efficiencyData, &m_powerData,
                                  &m_energyData);
  setupFixedSizePage(electricalParameterPage);

  TrainSpeedPage *trainSpeedPage = new TrainSpeedPage(this);
  setupFixedSizePage(trainSpeedPage);

  TractionEffortPage *tractionEffortPage = new TractionEffortPage(this);
  setupFixedSizePage(tractionEffortPage);

  TrainPowerPage *trainPowerPage = new TrainPowerPage(this, m_trainSimulation);
  setupFixedSizePage(trainPowerPage);
}

void MainWindow::navigateToPage(int pageIndex) {
  if (pageIndex >= 0 && pageIndex < stackedWidget->count()) {
    stackedWidget->setCurrentIndex(pageIndex);
  }
}
