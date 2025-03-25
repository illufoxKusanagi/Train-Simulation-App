#include "chart_widget.h"

ChartWidget::ChartWidget(QWidget *parent, QString chartTitle,
                         QString seriesName, TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_chartLayout(nullptr), m_chartWidget(nullptr),
      m_trainSimulation(trainSimulation), m_chartTitle(chartTitle) {
  mainLayout->setContentsMargins(0, 0, 0, 0);
  mainLayout->setSpacing(16);
  addSeries(seriesName, QColor(0, 114, 206));
  connect(m_trainSimulation, &TrainSimulation::simulationCompleted, this,
          &ChartWidget::onSimulationCompleted);
  connect(m_trainSimulation, &TrainSimulation::staticSimulationCompleted, this,
          &ChartWidget::onStaticSimulationCompleted);
  chartTitle.contains("Track") ? setupTable() : buildDummyLine(seriesName);
}

void ChartWidget::addSeries(const QString &name, const QColor &color) {
  if (!m_seriesToDisplay.contains(name)) {
    m_seriesToDisplay.append(name);
    m_seriesColors[name] = color;
  }
}

void ChartWidget::onSimulationCompleted() {
  if (m_chartTitle.contains("Dynamic") || m_chartTitle.contains("Distance")) {
    m_simulationType = Dynamic;
    updateChart();
  }
}

void ChartWidget::onStaticSimulationCompleted() {
  if (m_chartTitle.contains("Static")) {
    m_simulationType = Static;
    m_chartTitle.contains("Track") ? updateTable() : updateStaticChart();
  }
}

void ChartWidget::updateChart() {
  if (m_chart) {
    QList<QAbstractSeries *> seriesToRemove;
    for (QAbstractSeries *series : m_chart->series()) {
      QString name = series->name();
      if (name.contains("Dynamic"))
        seriesToRemove.append(series);
      else if (name.contains("Distance"))
        seriesToRemove.append(series);
    }

    for (QAbstractSeries *series : seriesToRemove) {
      m_chart->removeSeries(series);
    }
    if (m_chartTitle == "Dynamic Power") {
      setupDynamicPowerChart();
    } else if (m_chartTitle == "Dynamic Current") {
      setupDynamicCurrentChart();
    } else if (m_chartTitle == "Dynamic Speed") {
      setupDynamicSpeedChart();
    } else if (m_chartTitle == "Dynamic Traction Effort") {
      setupDynamicTractionChart();
    } else if (m_chartTitle == "Dynamic Energy") {
      setupDynamicEnergyChart();
    } else if (m_chartTitle == "Distance") {
      setupDistanceChart();
    }
    setupDynamicAxis();
  }
}

void ChartWidget::updateStaticChart() {
  if (m_chart) {
    QList<QAbstractSeries *> seriesToRemove;
    for (QAbstractSeries *series : m_chart->series()) {
      QString name = series->name();
      if (name.contains("Static"))
        seriesToRemove.append(series);
      else if (name.contains("Distance"))
        seriesToRemove.append(series);
      else if (name.startsWith("speed"))
        seriesToRemove.append(series);
    }
    for (QAbstractSeries *series : seriesToRemove) {
      m_chart->removeSeries(series);
    }
    if (m_chartTitle == "Static Power") {
      setupStaticPowerChart();
    } else if (m_chartTitle == "Static Current") {
      setupStaticCurrentChart();
    } else if (m_chartTitle == "Static Speed") {
      setupStaticSpeedChart();
    } else if (m_chartTitle == "Static Traction Effort") {
      setupStaticTractionChart();
    } else if (m_chartTitle == "Static Energy") {
      setupStaticEnergyChart();
    }
    setupStaticAxis();
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

void ChartWidget::setupTable() {
  QStringList dummyHeaders = {"Track distance", "Track distance on EB"};
  m_table = new QTableWidget();
  m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
  m_table->setColumnCount(dummyHeaders.size());
  m_table->setHorizontalHeaderLabels(dummyHeaders);
  m_table->setRowCount(0);
  m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  m_table->setStyleSheet(
      "QTableView {"
      "color : " +
      Colors::StandardBlack.name() + ";" + TextStyle::BodyMediumRegular() +
      "border: 1px solid " + Colors::Grey800.name() +
      "; "
      "}"
      "QTableView::item:selected{"
      "background-color: " +
      Colors::Primary300.name() +
      ";"
      "color: " +
      Colors::StandardBlack.name() +
      ";"
      "}"
      "QTableView::item:hover {"
      "background-color: " +
      Colors::Primary100.name() +
      ";"
      "}"
      "QHeaderView::section { background-color:" +
      Colors::Secondary400.name() + ";" + TextStyle::BodyMediumBold() +
      "color: white; padding: 6px; }");
  mainLayout->addWidget(m_table);
}

void ChartWidget::updateTable() {
  double normalTrackLength =
      m_trainSimulation->calculateNormalSimulationTrack();
  double delayTrackLength = m_trainSimulation->calculateDelaySimulationTrack();
  double safetyTrackLength =
      m_trainSimulation->calculateSafetySimulationTrack();
  double normalEmergencyTrackLength =
      m_trainSimulation->calculateEmergencyNormalSimulationTrack();
  double delayEmergencyTrackLength =
      m_trainSimulation->calculateEmergencyDelaySimulationTrack();
  double safetyEmergencyTrackLength =
      m_trainSimulation->calculateEmergencySafetySimulationTrack();
  QList<double> normalBraking = {normalTrackLength, delayTrackLength,
                                 safetyTrackLength};
  QList<double> emergencyBraking = {normalEmergencyTrackLength,
                                    delayEmergencyTrackLength,
                                    safetyEmergencyTrackLength};
  m_table->setRowCount(normalBraking.size());
  m_table->setVerticalHeaderLabels({"Normal", "Delay 3s", "Safety 20%"});
  for (int i = 0; i < normalBraking.size(); i++)
    m_table->setItem(i, 0,
                     new QTableWidgetItem(QString::number(normalBraking[i])));
  for (int i = 0; i < emergencyBraking.size(); i++)
    m_table->setItem(
        i, 1, new QTableWidgetItem(QString::number(emergencyBraking[i])));
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
  m_chart->legend()->setVisible(true);
  m_chart->legend()->setAlignment(Qt::AlignBottom);
  m_chart->legend()->setFont(QFont("Roboto", 10));
  m_chart->setPlotAreaBackgroundVisible(true);
  m_chart->setPlotAreaBackgroundBrush(QBrush(Colors::Secondary50));
  createChartButtons(chartView);
  mainLayout->addWidget(m_chartWidget);
}

void ChartWidget::createChartButtons(QChartView *chartView) {
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->setAlignment(Qt::AlignRight);
  buttonLayout->setSpacing(16);
  ButtonAction *saveButton = new ButtonAction(this, "Save Chart");
  ButtonAction *saveCurrentData = new ButtonAction(this, "Save this data");
  ButtonAction *saveAllData = new ButtonAction(this, "Save all data");
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
      else if (m_chartTitle == "Traction Effort" ||
               m_chartTitle == "Static Traction Effort")
        saveSuccessful = m_trainSimulation->saveTractionEffortData();
      else if (m_chartTitle == "Dynamic Speed" ||
               m_chartTitle == "Static Speed")
        saveSuccessful = m_trainSimulation->saveTrainSpeedData();
      else if (m_chartTitle == "Distance")
        saveSuccessful = m_trainSimulation->saveTrainTrackData();
      else if (m_chartTitle == "Dynamic Energy" ||
               m_chartTitle == "Static Energy")
        saveSuccessful = m_trainSimulation->saveEnergyConsumptionData();
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

void ChartWidget::setupDynamicSpeedChart() {
  QLineSeries *speedSeries = new QLineSeries();
  speedSeries->setName("Dynamic Speed");
  speedSeries->setPen(QPen(QColor(0, 114, 206), 2));
  const auto &speeds = m_trainSimulation->simulationDatas.trainSpeeds;
  const auto &time = m_trainSimulation->simulationDatas.timeTotal;
  for (int i = 0; i < time.size() && i < speeds.size(); ++i) {
    speedSeries->append(time[i], speeds[i]);
  }
  m_chart->addSeries(speedSeries);
}

void ChartWidget::setupDynamicTractionChart() {
  QLineSeries *speedSeries = new QLineSeries();
  speedSeries->setName("Dynamic F motor");
  speedSeries->setPen(QPen(QColor(0, 114, 206), 2));
  const auto &time = m_trainSimulation->simulationDatas.timeTotal;
  const auto &resistance = m_trainSimulation->simulationDatas.tractionEfforts;
  for (int i = 0; i < resistance.size() && i < time.size(); ++i) {
    speedSeries->append(time[i], resistance[i]);
  }
  m_chart->addSeries(speedSeries);
}

void ChartWidget::setupDynamicPowerChart() {
  QLineSeries *catenaryPowerSeries = new QLineSeries();
  QLineSeries *vvvfPowerSeries = new QLineSeries();
  catenaryPowerSeries->setName("Dynamic Catenary Power");
  vvvfPowerSeries->setName("Dynamic Max VVVF Power");
  catenaryPowerSeries->setPen(QPen(Colors::Primary500, 2));
  vvvfPowerSeries->setPen(QPen(Colors::Danger500, 2));
  const auto &times = m_trainSimulation->simulationDatas.timeTotal;
  const auto &catenaryPowers =
      m_trainSimulation->simulationDatas.catenaryPowers;
  const auto &vvvfPowers = m_trainSimulation->simulationDatas.vvvfPowers;

  for (int i = 0; i < times.size() && i < catenaryPowers.size(); ++i) {
    catenaryPowerSeries->append(times[i], catenaryPowers[i]);
    vvvfPowerSeries->append(times[i], vvvfPowers[i]);
  }
  m_chart->addSeries(catenaryPowerSeries);
  m_chart->addSeries(vvvfPowerSeries);
}

void ChartWidget::setupDynamicCurrentChart() {
  QLineSeries *catenaryCurrentSeries = new QLineSeries();
  QLineSeries *vvvfCurrentSeries = new QLineSeries();

  catenaryCurrentSeries->setName("Dynamic Catenary Current");
  vvvfCurrentSeries->setName("Dynamic VVVF Current");

  catenaryCurrentSeries->setPen(QPen(Colors::Primary500, 2));
  vvvfCurrentSeries->setPen(QPen(Colors::Danger500, 2));

  const auto &time = m_trainSimulation->simulationDatas.timeTotal;
  const auto &catenaryCurrents =
      m_trainSimulation->simulationDatas.catenaryCurrents;
  const auto &vvvfCurrents = m_trainSimulation->simulationDatas.vvvfCurrents;

  for (int i = 0; i < time.size() && i < catenaryCurrents.size(); ++i) {
    catenaryCurrentSeries->append(time[i], catenaryCurrents[i]);
    vvvfCurrentSeries->append(time[i], vvvfCurrents[i]);
  }

  m_chart->addSeries(catenaryCurrentSeries);
  m_chart->addSeries(vvvfCurrentSeries);
}

void ChartWidget::setupStaticPowerChart() {
  QLineSeries *catenaryPowerSeries = new QLineSeries();
  QLineSeries *vvvfPowerSeries = new QLineSeries();

  catenaryPowerSeries->setName("Static Catenary Power");
  vvvfPowerSeries->setName("Static VVVF Power");

  catenaryPowerSeries->setPen(QPen(Colors::Primary500, 2));
  vvvfPowerSeries->setPen(QPen(Colors::Danger500, 2));

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

void ChartWidget::setupStaticCurrentChart() {
  QLineSeries *catenaryCurrentSeries = new QLineSeries();
  QLineSeries *vvvfCurrentSeries = new QLineSeries();

  catenaryCurrentSeries->setName("Static Catenary Current");
  vvvfCurrentSeries->setName("Static VVVF Current");

  catenaryCurrentSeries->setPen(QPen(Colors::Primary500, 2));
  vvvfCurrentSeries->setPen(QPen(Colors::Danger500, 2));

  const auto &speeds = m_trainSimulation->simulationDatas.trainSpeeds;
  const auto &catenaryCurrents =
      m_trainSimulation->simulationDatas.catenaryCurrents;
  const auto &vvvfCurrents = m_trainSimulation->simulationDatas.vvvfCurrents;

  for (int i = 0; i < speeds.size() && i < catenaryCurrents.size(); ++i) {
    catenaryCurrentSeries->append(speeds[i], catenaryCurrents[i]);
    vvvfCurrentSeries->append(speeds[i], vvvfCurrents[i]);
  }

  m_chart->addSeries(catenaryCurrentSeries);
  m_chart->addSeries(vvvfCurrentSeries);
}

void ChartWidget::setupStaticSpeedChart() {
  QLineSeries *speedSeries = new QLineSeries();
  speedSeries->setName("Static Speed");
  speedSeries->setPen(QPen(Colors::Primary500, 2));
  const auto &speeds = m_trainSimulation->simulationDatas.trainSpeeds;
  const auto &distance = m_trainSimulation->simulationDatas.distanceTotal;
  for (int i = 0; i < distance.size() && i < speeds.size(); ++i) {
    speedSeries->append(distance[i], speeds[i]);
  }
  m_chart->addSeries(speedSeries);
}

void ChartWidget::setupStaticTractionChart() {
  QLineSeries *speedSeries = new QLineSeries();
  speedSeries->setName("Static F motor");
  speedSeries->setPen(QPen(Colors::Primary500, 2));
  const auto &speed = m_trainSimulation->simulationDatas.trainSpeeds;
  const auto &resistance = m_trainSimulation->simulationDatas.tractionEfforts;
  for (int i = 0; i < speed.size() && i < resistance.size(); ++i) {
    speedSeries->append(speed[i], resistance[i]);
  }
  m_chart->addSeries(speedSeries);
}

void ChartWidget::setupDynamicEnergyChart() {
  QLineSeries *energyMotorSeries = new QLineSeries();
  QLineSeries *energyPoweringSeries = new QLineSeries();
  QLineSeries *energyRegenSeries = new QLineSeries();
  QLineSeries *energyApsSeries = new QLineSeries();
  energyMotorSeries->setName("Dynamic Energy Motor");
  energyPoweringSeries->setName("Dynamic Energy Powering");
  energyRegenSeries->setName("Dynamic Energy Regen");
  energyApsSeries->setName("Dynamic Energy APS");
  energyMotorSeries->setPen(QPen(Colors::Primary500, 2));
  energyPoweringSeries->setPen(QPen(Colors::Danger500, 2));
  energyApsSeries->setPen(QPen(Colors::Warning600, 2));
  energyRegenSeries->setPen(QPen(Colors::Secondary400, 2));
  const auto &energyMotorData =
      m_trainSimulation->simulationDatas.energyConsumptions;
  const auto &energyPoweringData =
      m_trainSimulation->simulationDatas.energyPowerings;
  const auto &energyRegenData =
      m_trainSimulation->simulationDatas.energyRegenerations;
  const auto &energyApsData = m_trainSimulation->simulationDatas.energyAps;
  const auto &times = m_trainSimulation->simulationDatas.timeTotal;
  for (int i = 0; i < energyMotorData.size() && i < energyPoweringData.size();
       i++) {
    energyMotorSeries->append(times[i], energyMotorData[i]);
    energyPoweringSeries->append(times[i], energyPoweringData[i]);
    energyRegenSeries->append(times[i], energyRegenData[i]);
    energyApsSeries->append(times[i], energyApsData[i]);
  }
  m_chart->addSeries(energyMotorSeries);
  m_chart->addSeries(energyPoweringSeries);
  m_chart->addSeries(energyRegenSeries);
  m_chart->addSeries(energyApsSeries);
}

void ChartWidget::setupStaticEnergyChart() {
  QLineSeries *energyMotorSeries = new QLineSeries();
  QLineSeries *energyPoweringSeries = new QLineSeries();
  QLineSeries *energyApsSeries = new QLineSeries();
  energyMotorSeries->setName("Static Energy Motor");
  energyPoweringSeries->setName("Static Energy Powering");
  energyApsSeries->setName("Static Energy APS");
  energyMotorSeries->setPen(QPen(Colors::Primary500, 2));
  energyPoweringSeries->setPen(QPen(Colors::Danger500, 2));
  energyApsSeries->setPen(QPen(Colors::Warning600, 2));
  const auto &energyMotorData =
      m_trainSimulation->simulationDatas.energyConsumptions;
  const auto &energyPoweringData =
      m_trainSimulation->simulationDatas.energyPowerings;
  const auto &energyApsData = m_trainSimulation->simulationDatas.energyAps;
  const auto &speed = m_trainSimulation->simulationDatas.trainSpeeds;
  for (int i = 0; i < energyMotorData.size() && i < energyPoweringData.size();
       i++) {
    energyMotorSeries->append(speed[i], energyMotorData[i]);
    energyPoweringSeries->append(speed[i], energyPoweringData[i]);
    energyApsSeries->append(speed[i], energyApsData[i]);
  }
  m_chart->addSeries(energyMotorSeries);
  m_chart->addSeries(energyPoweringSeries);
  m_chart->addSeries(energyApsSeries);
}

void ChartWidget::setupDistanceChart() {
  QLineSeries *distanceSeries = new QLineSeries();
  distanceSeries->setName("Distance");
  distanceSeries->setPen(QPen(Colors::Primary500, 2));
  const auto &distance = m_trainSimulation->simulationDatas.distanceTotal;
  const auto &times = m_trainSimulation->simulationDatas.timeTotal;
  for (int i = 0; i < distance.size(); i++) {
    distanceSeries->append(times[i], distance[i]);
  }
  m_chart->addSeries(distanceSeries);
}

void ChartWidget::setupStaticAxis() {
  double maxValue;
  double roundedMaxValue;
  if (m_chart->series().isEmpty())
    return;
  if (m_simulationType != Static)
    return;
  m_chart->createDefaultAxes();
  // Set proper axis labels
  QValueAxis *axisX =
      qobject_cast<QValueAxis *>(m_chart->axes(Qt::Horizontal).first());
  QValueAxis *axisY =
      qobject_cast<QValueAxis *>(m_chart->axes(Qt::Vertical).first());
  if (!axisX || !axisY)
    return;

  axisX->setMinorTickCount(1);
  axisX->setLabelFormat("%.0f");
  axisY->setMinorTickCount(4);
  axisY->setLabelFormat("%.0f");

  if (m_chartTitle.contains("Static Speed")) {
    axisX->setRange(
        0, (1.2 * m_trainSimulation->simulationDatas.distanceTotal.last()));
    axisX->setTitleText("Distance (m)");
  } else if (m_chartTitle.contains("Static")) {
    axisX->setRange(
        0, 1.2 * m_trainSimulation->simulationDatas.trainSpeeds.last());
    axisX->setTitleText("Speed (km/h)");
  }
  if (m_chartTitle.contains("Static Power")) {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.catenaryPowers.begin(),
                   m_trainSimulation->simulationDatas.catenaryPowers.end()) *
               1.2;
    roundedMaxValue = ceil(maxValue / 100) * 100;
    axisY->setRange(0, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 1000) + 1);
    axisY->setTitleText("Power (kW)");
  } else if (m_chartTitle.contains("Static Current")) {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.catenaryCurrents.begin(),
                   m_trainSimulation->simulationDatas.catenaryCurrents.end()) *
               1.1;
    roundedMaxValue = ceil(maxValue / 100) * 100;
    axisY->setRange(0, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 1000) + 1);
    axisY->setTitleText("Current (A)");
  } else if (m_chartTitle.contains("Static Speed")) {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.trainSpeeds.begin(),
                   m_trainSimulation->simulationDatas.trainSpeeds.end()) *
               1.1;
    roundedMaxValue = ceil(maxValue / 10) * 10;
    axisY->setRange(0, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 100) + 1);
    axisY->setTitleText("Speed (km/h)");
  } else if (m_chartTitle.contains("Static Traction Effort")) {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.tractionEfforts.begin(),
                   m_trainSimulation->simulationDatas.tractionEfforts.end()) *
               1.1;
    roundedMaxValue = ceil(maxValue / 100) * 100;
    axisY->setRange(0, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 100) + 1);
    axisY->setTitleText("Traction Effort (kN)");
  } else if (m_chartTitle.contains("Static Energy")) {
    maxValue =
        *std::max_element(
            m_trainSimulation->simulationDatas.energyConsumptions.begin(),
            m_trainSimulation->simulationDatas.energyConsumptions.end()) *
        1.1;
    roundedMaxValue = ceil(maxValue / 10) * 10;
    axisY->setRange(0, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 10) + 1);
    axisY->setTitleText("Energy (kW)");
  }
}

void ChartWidget::setupDynamicAxis() {
  double maxValue;
  double roundedMaxValue;
  if (m_chart->series().isEmpty())
    return;
  if (m_simulationType != Dynamic)
    return;
  m_chart->createDefaultAxes();

  // Set proper axis labels
  QValueAxis *axisX =
      qobject_cast<QValueAxis *>(m_chart->axes(Qt::Horizontal).first());
  QValueAxis *axisY =
      qobject_cast<QValueAxis *>(m_chart->axes(Qt::Vertical).first());

  if (!axisX || !axisY)
    return;
  // For X axis
  axisX->setMinorTickCount(1);   // Minor ticks between major ticks
  axisX->setLabelFormat("%.0f"); // No decimal places
  axisY->setMinorTickCount(4);   // Minor ticks for Y-axis
  axisY->setLabelFormat("%.0f"); // No decimal places
  if (m_chartTitle.contains("Dynamic") || m_chartTitle.contains("Distance")) {
    axisX->setRange(0,
                    1.2 * m_trainSimulation->simulationDatas.timeTotal.last());
    axisX->setTitleText("Time (s)");
  } else {
    axisX->setRange(
        0, 1.2 * m_trainSimulation->simulationDatas.trainSpeeds.last());
    axisX->setTitleText("Speed (km/h)");
  }

  if (m_chartTitle.contains("Dynamic Power")) {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.catenaryPowers.begin(),
                   m_trainSimulation->simulationDatas.catenaryPowers.end()) *
               1.2;
    roundedMaxValue = ceil(maxValue / 100) * 100;
    axisY->setRange(0, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 1000) + 1);
    axisY->setTitleText("Power (kW)");
  } else if (m_chartTitle.contains("Dynamic Current")) {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.catenaryCurrents.begin(),
                   m_trainSimulation->simulationDatas.catenaryCurrents.end()) *
               1.1;
    roundedMaxValue = ceil(maxValue / 100) * 100;
    axisY->setRange(0, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 1000) + 1);
    axisY->setTitleText("Current (A)");
  } else if (m_chartTitle.contains("Dynamic Speed")) {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.trainSpeeds.begin(),
                   m_trainSimulation->simulationDatas.trainSpeeds.end()) *
               1.1;
    roundedMaxValue = ceil(maxValue / 10) * 10;
    axisY->setRange(0, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 20) + 1);
    axisY->setTitleText("Speed (km/h)");
  } else if (m_chartTitle.contains("Dynamic Traction Effort")) {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.tractionEfforts.begin(),
                   m_trainSimulation->simulationDatas.tractionEfforts.end()) *
               1.1;
    roundedMaxValue = ceil(maxValue / 100) * 100;
    axisY->setRange(0, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 100) + 1);
    axisY->setTitleText("Traction Effort (kN)");
  } else if (m_chartTitle.contains("Dynamic Energy")) {
    maxValue =
        *std::max_element(
            m_trainSimulation->simulationDatas.energyConsumptions.begin(),
            m_trainSimulation->simulationDatas.energyConsumptions.end()) *
        1.1;
    roundedMaxValue = ceil(maxValue / 10) * 10;
    axisY->setRange(0, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 10) + 1);
    axisY->setTitleText("Energy (kW)");
  } else if (m_chartTitle.contains("Distance")) {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.distanceTotal.begin(),
                   m_trainSimulation->simulationDatas.distanceTotal.end()) *
               1.1;
    roundedMaxValue = ceil(maxValue / 100) * 100;
    axisY->setRange(0, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 100) + 1);
    axisY->setTitleText("Distance (m)");
  }
}
