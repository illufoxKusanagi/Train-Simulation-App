#include "chart_widget.h"

ChartWidget::ChartWidget(QString chartTitle, QString seriesName,
                         QWidget *parent, TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_chartLayout(nullptr), m_chartWidget(nullptr),
      m_trainSimulation(trainSimulation), m_chartTitle(chartTitle) {
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(16);
  buildDummyLine(seriesName);
}

void ChartWidget::buildDummyLine(QString seriesName) {
  QLineSeries *series = new QLineSeries();
  series->setName(seriesName);

  series->append(0.0, 0.0);
  series->append(1.1, 2.1);
  series->append(1.9, 3.3);
  series->append(2.1, 2.1);
  series->append(2.9, 4.9);
  series->append(3.4, 3.0);
  series->append(4.1, 3.3);
  setupChart(series, m_chartTitle);
}

void ChartWidget::setupChart(QLineSeries *series, QString title) {
  m_chartWidget = new QWidget();
  QChart *chart = new QChart();
  chart->addSeries(series);
  chart->setTitle(title);
  chart->createDefaultAxes();

  QFont titleFont;
  titleFont.setPixelSize(20);
  titleFont.setWeight(QFont::DemiBold);
  chart->setTitleFont(titleFont);
  chart->setTitleBrush(QBrush(Colors::StandardBlack));

  QChartView *chartView = new QChartView(chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setFixedSize(800, 450);

  m_chartLayout = new QVBoxLayout(m_chartWidget);
  m_chartLayout->setContentsMargins(0, 0, 0, 0);
  m_chartLayout->setSpacing(16);
  m_chartLayout->addWidget(chartView, 0, Qt::AlignCenter);
  chartView->setStyleSheet("border: 1px solid " + Colors::Grey100.name() +
                           ";"
                           "border-radius: 12px;");
  createChartButtons(chartView);
  mainLayout->addWidget(m_chartWidget);
}

void ChartWidget::createChartButtons(QChartView *chartView) {
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
  connect(saveCurrentData, &QPushButton::clicked, this, [this]() {
    try {
      if (m_trainSimulation->trainSpeeds.isEmpty()) {
        QMessageBox::warning(
            this, "No Data",
            "No simulation data to save. Please run a simulation first.");
        return;
      }
      bool saveSuccessful = false;
      if (m_chartTitle == "Train Power")
        saveSuccessful = m_trainSimulation->saveTrainPowerData();
      else if (m_chartTitle == "Traction Effort")
        saveSuccessful = m_trainSimulation->saveTractionEffortData();
      else
        saveSuccessful = m_trainSimulation->saveTrainSpeedData();
      if (saveSuccessful)
        QMessageBox::information(this, "Success", "Data saved successfully!");
    } catch (const std::exception &e) {
      QMessageBox::critical(this, "Error",
                            QString("Failed to save data: %1").arg(e.what()));
    }
  });
  connect(saveAllData, &QPushButton::clicked, this, [this]() {
    try {
      if (m_trainSimulation->trainSpeeds.isEmpty()) {
        QMessageBox::warning(
            this, "No Data",
            "No simulation data to save. Please run a simulation first.");
        return;
      }
      m_trainSimulation->printSimulationDatas();
    } catch (const std::exception &e) {
      QMessageBox::critical(this, "Error",
                            QString("Failed to save data: %1").arg(e.what()));
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

  m_chartLayout->addLayout(buttonLayout);
}
