#include "output_page.h"

OutputPage::OutputPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      stackedWidget(new QStackedWidget(this)),
      chartLayout(new QVBoxLayout(this)), buttonLayout(new QHBoxLayout(this)) {

  // // Example signals
  // connect(prevButton, &QPushButton::clicked, this, [=]() {
  //   // TODO: show previous page logic
  // });
  // connect(nextButton, &QPushButton::clicked, this, [=]() {
  //   // TODO: show next page logic
  // });

  ButtonAction *runButton = new ButtonAction("Run Simulation", "yes", this);
  runButton->setEnabled(true);
  mainLayout->addWidget(runButton);

  setupFirstPage();

  setupPagination();

  setLayout(mainLayout);
}

void OutputPage::setupFirstPage() {
  QLineSeries *series = new QLineSeries();
  series->setName("Line");

  series->append(0.0, 0.0);
  series->append(1.1, 2.1);
  series->append(1.9, 3.3);
  series->append(2.1, 2.1);
  series->append(2.9, 4.9);
  series->append(3.4, 3.0);
  series->append(4.1, 3.3);

  setupChart(series);
}

void OutputPage::setupSecondPage() {}

void OutputPage::setupThirdPage() {}

void OutputPage::setupChart(QLineSeries *series) {

  // Create chart, add series
  QChart *chart = new QChart();
  chart->addSeries(series);
  chart->setTitle("Sample Chart");
  chart->createDefaultAxes();

  // Create chart view
  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartLayout->addWidget(chartView);

  createChartButtons(chartView);
  mainLayout->addLayout(chartLayout);
}

void OutputPage::createChartButtons(QChartView *chartView) {
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setAlignment(Qt::AlignRight);
  buttonLayout->setStretch(0, 1);
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

  saveButton->setFixedSize(150, 40);
  saveCurrentData->setFixedSize(150, 40);
  saveAllData->setFixedSize(150, 40);

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

  mainLayout->addLayout(buttonLayout);
}
