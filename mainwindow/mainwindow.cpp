#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  QWidget *centralWidget = new QWidget(this);
  QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  leftPanel = new LeftPanel(this);
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

MainWindow::~MainWindow() {}

void MainWindow::setupPages() {
  const int PAGE_WIDTH = 960;
  const int PAGE_HEIGHT = 720;
  auto setupFixedSizePage = [this, PAGE_WIDTH, PAGE_HEIGHT](QWidget *page) {
    page->setFixedSize(PAGE_WIDTH, PAGE_HEIGHT);
    page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    stackedWidget->addWidget(page);
  };
  ConstantValuesPage *constantValuesPage = new ConstantValuesPage(this);
  setupFixedSizePage(constantValuesPage);

  TrainParameterPage *trainParameterPage = new TrainParameterPage(this);
  setupFixedSizePage(trainParameterPage);

  RunningParameterPage *runningParameterPage = new RunningParameterPage(this);
  setupFixedSizePage(runningParameterPage);

  TrackParameterPage *trackParameterPage = new TrackParameterPage(this);
  setupFixedSizePage(trackParameterPage);

  ElectricalParameterPage *electricalParameterPage =
      new ElectricalParameterPage(this);
  setupFixedSizePage(electricalParameterPage);

  OutputPage *outputPage = new OutputPage(this);
  setupFixedSizePage(outputPage);

  TrainSpeedPage *trainSpeedPage = new TrainSpeedPage(this);
  setupFixedSizePage(trainSpeedPage);

  TrainPowerPage *trainPowerPage = new TrainPowerPage(this);
  setupFixedSizePage(trainPowerPage);

  TractionEffortPage *tractionEffortPage = new TractionEffortPage(this);
  setupFixedSizePage(tractionEffortPage);
}

void MainWindow::navigateToPage(int pageIndex) {
  if (pageIndex >= 0 && pageIndex < stackedWidget->count()) {
    stackedWidget->setCurrentIndex(pageIndex);
  }
}
