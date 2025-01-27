#include "mainwindow.h"
#include "../pages/constant_values_page.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  // Inisialisasi widget utama
  QWidget *centralWidget = new QWidget(this);
  QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(0);

  // Setup Left Panel
  leftPanel = new LeftPanel(this);
  connect(leftPanel, &LeftPanel::navigateToPage, this,
          &MainWindow::navigateToPage);

  // Setup Pages
  stackedWidget = new QStackedWidget(this);
  setupPages();

  // Tambahkan Left Panel dan Pages ke Layout Utama
  mainLayout->addWidget(leftPanel);
  mainLayout->addWidget(stackedWidget);
  centralWidget->setLayout(mainLayout);
  setCentralWidget(centralWidget);

  // Atur halaman awal
  navigateToPage(0);
}

MainWindow::~MainWindow() {}

void MainWindow::setupPages() {
  // Tambahkan halaman-halaman ke dalam stackedWidget
  // QWidget *constantValuesPage = new QWidget(this);
  // QVBoxLayout *constantLayout = new QVBoxLayout(constantValuesPage);
  // constantLayout->addWidget(new QLabel("Constant Values Page", this));
  // constantValuesPage->setLayout(constantLayout);
  ConstantValuesPage *constantValuesPage = new ConstantValuesPage(this);
  stackedWidget->addWidget(constantValuesPage);

  QWidget *trainParameterPage = new QWidget(this);
  QVBoxLayout *trainLayout = new QVBoxLayout(trainParameterPage);
  trainLayout->addWidget(new QLabel("Train Parameter Page", this));
  trainParameterPage->setLayout(trainLayout);
  stackedWidget->addWidget(trainParameterPage);

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
