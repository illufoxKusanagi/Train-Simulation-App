#include "chart_widget.h"

ChartWidget::ChartWidget(QString chartTitle, QString seriesName,
                         QWidget *parent, TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_chartLayout(nullptr), m_chartWidget(nullptr),
      m_trainSimulation(trainSimulation), m_chartTitle(chartTitle) {
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(16);
  addSeries(seriesName, QColor(0, 114, 206));
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &ChartWidget::onSimulationCompleted);
  buildDummyLine(seriesName);
}

void ChartWidget::addSeries(const QString &name, const QColor &color) {
  if (!m_seriesToDisplay.contains(name)) {
    m_seriesToDisplay.append(name);
    m_seriesColors[name] = color;
  }
}

void ChartWidget::onSimulationCompleted() { updateChart(); }

void ChartWidget::updateChart() {
  if (m_chart) {
    m_chart->removeAllSeries();
    if (m_chartTitle == "Dynamic Power") {
      if (m_seriesToDisplay.contains("Dynamic Catenary Power")) {
        QLineSeries *catenaryPowerSeries = new QLineSeries();
        QLineSeries *vvvfPowerSeries = new QLineSeries();
        catenaryPowerSeries->setName("Dynamic Catenary Power");
        vvvfPowerSeries->setName("Dynamic Max VVVF Power");
        catenaryPowerSeries->setPen(
            QPen(m_seriesColors.value("Dynamic Catenary Power", Qt::blue), 2));
        catenaryPowerSeries->setPen(
            QPen(m_seriesColors.value("Dynamic Catenary Power", Qt::red), 2));
        const auto &speeds = m_trainSimulation->simulationDatas.trainSpeeds;
        const auto &catenaryPowers =
            m_trainSimulation->simulationDatas.catenaryPowers;
        const auto &vvvfPowers = m_trainSimulation->simulationDatas.vvvfPowers;

        for (int i = 0; i < speeds.size() && i < catenaryPowers.size(); ++i) {
          catenaryPowerSeries->append(speeds[i], catenaryPowers[i]);
          vvvfPowerSeries->append(speeds[i], vvvfPowers[i]);
        }
        m_chart->addSeries(catenaryPowerSeries);
        m_chart->addSeries(vvvfPowerSeries);
      }
    }
    if (m_chartTitle == "")
      return;

    m_chart->createDefaultAxes();
  }
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
  m_chart = new QChart();
  m_chart->addSeries(series);
  m_chart->setTitle(title);
  m_chart->createDefaultAxes();

  QFont titleFont;
  titleFont.setPixelSize(20);
  titleFont.setWeight(QFont::DemiBold);
  m_chart->setTitleFont(titleFont);
  m_chart->setTitleBrush(QBrush(Colors::StandardBlack));

  QChartView *chartView = new QChartView(m_chart);
  chartView->setRenderHint(QPainter::Antialiasing);
  chartView->setFixedSize(800, 450);
  m_chartLayout = new QVBoxLayout(m_chartWidget);
  m_chartLayout->setContentsMargins(0, 0, 0, 0);
  m_chartLayout->setSpacing(16);
  m_chartLayout->addWidget(chartView, 0, Qt::AlignCenter);
  chartView->setStyleSheet("border: 1px solid " + Colors::Grey100.name() +
                           ";"
                           "border-radius: 12px;");
  // Add to setupChart method in chart_widget.cpp
  m_chart->legend()->setVisible(true);
  m_chart->legend()->setAlignment(Qt::AlignBottom);
  m_chart->legend()->setFont(QFont("Roboto", 10));

  // Make the chart's plot area fill the view
  m_chart->setPlotAreaBackgroundVisible(true);
  m_chart->setPlotAreaBackgroundBrush(QBrush(Colors::Secondary100));
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
      if (m_trainSimulation->simulationDatas.trainSpeeds.isEmpty()) {
        QMessageBox::warning(
            this, "No Data",
            "No simulation data to save. Please run a simulation first.");
        return;
      }
      bool saveSuccessful = false;
      if (m_chartTitle == "Dynamic Power" || m_chartTitle == "Static Power" ||
          m_chartTitle == "Dynamic Current" || m_chartTitle == "Static Current")
        saveSuccessful = m_trainSimulation->saveTrainPowerData();
      // else if (m_chartTitle == "Static Traction" ||
      //          m_chartTitle == "Dynamic Traction")
      else if (m_chartTitle == "Traction Effort")
        saveSuccessful = m_trainSimulation->saveTractionEffortData();
      // else if (m_chartTitle == "Dynamic Speed" ||
      //          m_chartTitle == "Static Speed")
      else if (m_chartTitle == "Train Speed")
        saveSuccessful = m_trainSimulation->saveTrainSpeedData();
      // else if (m_chartTitle == "Dynamic Track" ||
      //          m_chartTitle == "Static Track")
      else if (m_chartTitle == "Distance")
        saveSuccessful = m_trainSimulation->saveTrainTrackData();
      if (saveSuccessful)
        QMessageBox::information(this, "Success", "Data saved successfully!");

    } catch (const std::exception &e) {
      QMessageBox::critical(this, "Error",
                            QString("Failed to save data: %1").arg(e.what()));
    }
  });
  connect(saveAllData, &QPushButton::clicked, this, [this]() {
    try {
      if (m_trainSimulation->simulationDatas.trainSpeeds.isEmpty()) {
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