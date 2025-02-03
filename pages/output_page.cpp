#include "output_page.h"

OutputPage::OutputPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      stackedWidget(new QStackedWidget(this)), chartLayout(nullptr) {

  // // Example signals
  // connect(prevButton, &QPushButton::clicked, this, [=]() {
  //   // TODO: show previous page logic
  // });
  // connect(nextButton, &QPushButton::clicked, this, [=]() {
  //   // TODO: show next page logic
  // });

  QHBoxLayout *runButtonLayout = new QHBoxLayout();
  ButtonAction *runButton = new ButtonAction("Run Simulation", "yes", this);
  runButton->setEnabled(true);
  runButtonLayout->addWidget(runButton);
  runButton->setFixedSize(150, 40);
  runButtonLayout->setAlignment(Qt::AlignBottom);
  mainLayout->addLayout(runButtonLayout);
  mainLayout->addWidget(stackedWidget);

  setupFirstPage();
  setupSecondPage();

  setupPagination();

  setLayout(mainLayout);
}

void OutputPage::setupFirstPage() {
  QWidget *page1 = new QWidget();
  QVBoxLayout *layout1 = new QVBoxLayout(page1);
  layout1->setAlignment(Qt::AlignCenter);
  layout1->setSpacing(32);
  QLineSeries *series = new QLineSeries();
  series->setName("Speed");

  series->append(0.0, 0.0);
  series->append(1.1, 2.1);
  series->append(1.9, 3.3);
  series->append(2.1, 2.1);
  series->append(2.9, 4.9);
  series->append(3.4, 3.0);
  series->append(4.1, 3.3);

  setupChart(series);
  layout1->addLayout(chartLayout);

  page1->setLayout(layout1);
  stackedWidget->addWidget(page1);
}

void OutputPage::setupSecondPage() {
  QWidget *page2 = new QWidget;
  QVBoxLayout *layout2 = new QVBoxLayout(page2);
  layout2->setAlignment(Qt::AlignCenter);
  layout2->setSpacing(32);
  QLineSeries *series = new QLineSeries();
  series->setName("Voltage");
  series->append(0.0, 0.0);
  series->append(1.1, 2.1);
  series->append(4.1, 3.3);

  setupChart(series); // Reuses chartView, chartLayout
  layout2->addLayout(chartLayout);

  page2->setLayout(layout2);
  stackedWidget->addWidget(page2);
}

void OutputPage::setupThirdPage() {}

void OutputPage::setupChart(QLineSeries *series) {

  // Create chart, add series
  QChart *chart = new QChart();
  chart->addSeries(series);
  chart->setTitle("Mass to Speed graph");
  chart->createDefaultAxes();

  // Create chart view
  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setFixedSize(700, 500);

  chartLayout = new QVBoxLayout();
  chartLayout->addWidget(chartView, 0, Qt::AlignHCenter);

  createChartButtons(chartView);
}

void OutputPage::createChartButtons(QChartView *chartView) {
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setAlignment(Qt::AlignRight);
  buttonLayout->setSpacing(16);
  ButtonAction *saveButton = new ButtonAction("Save Chart", "normal", this);
  ButtonAction *saveCurrentData =
      new ButtonAction("Save this data", "normal", this);
  ButtonAction *saveAllData = new ButtonAction("Save all data", "normal", this);
  connect(saveButton, &QPushButton::clicked, this, [this, chartView]() {
    QString filePath = QFileDialog::getSaveFileName(
        this, tr("Save Chart"), QDir::homePath(),
        tr("Images (*.png *.jpg *.bmp);;All Files (*)"));

    if (!filePath.isEmpty()) {
      QPixmap pixmap = chartView->grab();
      if (!pixmap.save(filePath)) {
        QMessageBox::warning(
            this, tr("Save Failed"),
            tr("Could not save the chart to %1").arg(filePath));
      }
    }
  });
  saveButton->setEnabled(true);
  saveCurrentData->setEnabled(true);
  saveAllData->setEnabled(true);

  saveButton->setFixedSize(150, 48);
  saveCurrentData->setFixedSize(150, 48);
  saveAllData->setFixedSize(150, 48);

  buttonLayout->addWidget(saveButton);
  buttonLayout->addWidget(saveCurrentData);
  buttonLayout->addWidget(saveAllData);

  chartLayout->addLayout(buttonLayout);
}

void OutputPage::createActionButtons() {}

void OutputPage::setupPagination() {
  // Pagination / buttons
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  ButtonAction *prevButton = new ButtonAction("Prev", "normal", this);
  ButtonAction *nextButton = new ButtonAction("Next", "normal", this);
  buttonLayout->addWidget(prevButton);
  buttonLayout->addWidget(nextButton);

  prevButton->setEnabled(true);
  nextButton->setEnabled(true);

  connect(prevButton, &QPushButton::clicked, this, [this]() {
    int index = stackedWidget->currentIndex();
    if (index > 0)
      stackedWidget->setCurrentIndex(index - 1);
  });

  connect(nextButton, &QPushButton::clicked, this, [this]() {
    int index = stackedWidget->currentIndex();
    if (index < stackedWidget->count() - 1)
      stackedWidget->setCurrentIndex(index + 1);
  });

  mainLayout->addLayout(buttonLayout);
}
