#include "output_page.h"

OutputPage::OutputPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      stackedWidget(new QStackedWidget(this)), chartLayout(nullptr) {

  QWidget *runButtonWidget = new QWidget();
  QHBoxLayout *runButtonLayout = new QHBoxLayout(runButtonWidget);
  ButtonAction *runButton = new ButtonAction("Run Simulation", "yes", this);
  runButton->setEnabled(true);
  runButtonLayout->addWidget(runButton);
  runButtonLayout->setAlignment(Qt::AlignLeft);
  runButton->setFixedSize(200, 48);
  mainLayout->addWidget(runButtonWidget);
  mainLayout->addWidget(stackedWidget);
  setupFirstPage();
  setupSecondPage();
  setupThirdPage();
  setupPagination();

  setLayout(mainLayout);
}

void OutputPage::setupFirstPage() {
  QWidget *page1 = new QWidget();
  QVBoxLayout *layout1 = new QVBoxLayout(page1);
  layout1->setAlignment(Qt::AlignCenter);
  layout1->setSpacing(0);
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
  layout2->setSpacing(0);
  QLineSeries *series = new QLineSeries();
  series->setName("Voltage");
  series->append(0.0, 0.0);
  series->append(1.1, 2.1);
  series->append(4.1, 3.3);

  setupChart(series);
  layout2->addLayout(chartLayout);

  page2->setLayout(layout2);
  stackedWidget->addWidget(page2);
}

void OutputPage::setupThirdPage() {
  QWidget *page3 = new QWidget();
  QVBoxLayout *layout3 = new QVBoxLayout(page3);
  layout3->setAlignment(Qt::AlignCenter);
  layout3->setSpacing(0);

  QLineSeries *series = new QLineSeries();
  series->setName("Current");

  series->append(0.0, 0.0);
  series->append(1.0, 1.5);
  series->append(2.0, 3.0);
  series->append(3.0, 2.5);
  series->append(4.0, 4.0);

  setupChart(series);
  layout3->addLayout(chartLayout);

  page3->setLayout(layout3);
  stackedWidget->addWidget(page3);
}

void OutputPage::setupChart(QLineSeries *series) {

  QChart *chart = new QChart();
  chart->addSeries(series);
  chart->setTitle("Mass to Speed graph");
  chart->createDefaultAxes();

  chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setFixedSize(600, 450);

  chartLayout = new QVBoxLayout();
  chartLayout->setContentsMargins(0, 0, 0, 0);
  chartLayout->addWidget(chartView, 0, Qt::AlignCenter);
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
