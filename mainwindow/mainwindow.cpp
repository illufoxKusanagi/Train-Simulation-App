#include "mainwindow.h"
#include "../pages/constant_values_page.h"
#include "../pages/train_parameter_page.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  QWidget *centralWidget = new QWidget(this);
  QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  leftPanel = new LeftPanel(this);
  connect(leftPanel, &LeftPanel::navigateToPage, this,
          &MainWindow::navigateToPage);

  stackedWidget = new QStackedWidget(this);
  setupPages();

  mainLayout->addWidget(leftPanel);
  mainLayout->addWidget(stackedWidget);
  centralWidget->setLayout(mainLayout);
  setCentralWidget(centralWidget);
  navigateToPage(0);
}

MainWindow::~MainWindow() {}

void MainWindow::setupPages() {
  const int PAGE_WIDTH = 700;
  const int PAGE_HEIGHT = 500;
  // Untuk mengatur ukuran page
  auto setupFixedSizePage = [this, PAGE_WIDTH, PAGE_HEIGHT](QWidget *page) {
    page->setFixedSize(PAGE_WIDTH, PAGE_HEIGHT);
    page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    stackedWidget->addWidget(page);
  };
  ConstantValuesPage *constantValuesPage = new ConstantValuesPage(this);
  setupFixedSizePage(constantValuesPage);

  TrainParameterPage *trainParameterPage = new TrainParameterPage(this);
  setupFixedSizePage(trainParameterPage);

  // QWidget *trainParameterPage = new QWidget(this);
  // QVBoxLayout *trainLayout = new QVBoxLayout(trainParameterPage);
  // trainLayout->addWidget(new QLabel("Train Parameter Page", this));
  // trainParameterPage->setLayout(trainLayout);
  // stackedWidget->addWidget(trainParameterPage);

  QWidget *runningParameterPage = new QWidget(this);
  QVBoxLayout *runningLayout = new QVBoxLayout(runningParameterPage);
  runningLayout->addWidget(new QLabel("Running Parameter Page", this));
  runningParameterPage->setLayout(runningLayout);
  stackedWidget->addWidget(runningParameterPage);

  QWidget *trackParameterPage = new QWidget(this);
  QVBoxLayout *trackLayout = new QVBoxLayout(trackParameterPage);
  trackLayout->addWidget(new QLabel("Track Parameter Page", this));
  trackParameterPage->setLayout(trackLayout);
  stackedWidget->addWidget(trackParameterPage);

  QWidget *electricalParameterPage = new QWidget(this);
  QVBoxLayout *electricalLayout = new QVBoxLayout(electricalParameterPage);
  electricalLayout->addWidget(new QLabel("Electrical Parameter Page", this));
  electricalParameterPage->setLayout(electricalLayout);
  stackedWidget->addWidget(electricalParameterPage);

  QWidget *outputPage = new QWidget(this);
  QVBoxLayout *outputLayout = new QVBoxLayout(outputPage);
  outputLayout->addWidget(new QLabel("Output Page", this));
  outputPage->setLayout(outputLayout);
  stackedWidget->addWidget(outputPage);
}

void MainWindow::navigateToPage(int pageIndex) {
  if (pageIndex >= 0 && pageIndex < stackedWidget->count()) {
    stackedWidget->setCurrentIndex(pageIndex);
  }
}
