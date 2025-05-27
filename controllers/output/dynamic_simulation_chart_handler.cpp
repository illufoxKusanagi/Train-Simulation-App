#include "dynamic_simulation_chart_handler.h"

DynamicSimulationChartHandler::DynamicSimulationChartHandler(
    TrainSimulationHandler *trainSimulation, QChart *chart, QString &chartTitle,
    SaveButtonHandler::SimulationType *simulationType)
    : m_trainSimulation(trainSimulation), m_chart(chart),
      m_chartTitle(chartTitle), m_simulationType(simulationType) {}

void DynamicSimulationChartHandler::updateChart() {
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

void DynamicSimulationChartHandler::setupDynamicSpeedChart() {
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

void DynamicSimulationChartHandler::setupDynamicTractionChart() {
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

void DynamicSimulationChartHandler::setupDynamicPowerChart() {
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

void DynamicSimulationChartHandler::setupDynamicCurrentChart() {
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

void DynamicSimulationChartHandler::setupDynamicEnergyChart() {
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

void DynamicSimulationChartHandler::setupDistanceChart() {
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

void DynamicSimulationChartHandler::setupDynamicAxis() {
  double maxValue;
  double minValue;
  double roundedMaxValue;
  double roundedMinValue;
  if (m_chart->series().isEmpty())
    return;
  if (*m_simulationType != SaveButtonHandler::Dynamic)
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
    maxValue =
        *std::max_element(m_trainSimulation->simulationDatas.timeTotal.begin(),
                          m_trainSimulation->simulationDatas.timeTotal.end()) *
        1.1;
    roundedMaxValue = ceil(maxValue / 100) * 100;
    axisX->setRange(0, roundedMaxValue);
    axisX->setTickCount(ceil(roundedMaxValue / 100) + 1);
    axisX->setTitleText("Time (s)");
  } else {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.trainSpeeds.begin(),
                   m_trainSimulation->simulationDatas.trainSpeeds.end()) *
               1.1;
    roundedMaxValue = ceil(maxValue / 100) * 100;
    axisX->setRange(0, roundedMaxValue);
    axisX->setTickCount((ceil(roundedMaxValue / 100) + 1));
    axisX->setTitleText("Speed (km/h)");
  }

  if (m_chartTitle.contains("Dynamic Power")) {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.catenaryPowers.begin(),
                   m_trainSimulation->simulationDatas.catenaryPowers.end()) *
               1.1;
    minValue = *std::min_element(
                   m_trainSimulation->simulationDatas.catenaryPowers.begin(),
                   m_trainSimulation->simulationDatas.catenaryPowers.end()) *
               1.1;
    roundedMaxValue = ceil(maxValue / 100) * 100;
    roundedMinValue = floor(minValue / 100) * 100;
    if (minValue > 0) {
      roundedMinValue = 0;
    }
    axisY->setRange(roundedMinValue, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 1000) + 1);
    axisY->setTitleText("Power (kW)");
  } else if (m_chartTitle.contains("Dynamic Current")) {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.catenaryCurrents.begin(),
                   m_trainSimulation->simulationDatas.catenaryCurrents.end()) *
               1.1;
    minValue = *std::min_element(
                   m_trainSimulation->simulationDatas.catenaryCurrents.begin(),
                   m_trainSimulation->simulationDatas.catenaryCurrents.end()) *
               1.1;
    roundedMaxValue = ceil(maxValue / 100) * 100;
    roundedMinValue = floor(minValue / 100) * 100;
    if (minValue > 0) {
      roundedMinValue = 0;
    }
    axisY->setRange(roundedMinValue, roundedMaxValue);
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
    minValue = *std::min_element(
                   m_trainSimulation->simulationDatas.tractionEfforts.begin(),
                   m_trainSimulation->simulationDatas.tractionEfforts.end()) *
               1.1;
    roundedMaxValue = ceil(maxValue / 100) * 100;
    roundedMinValue = floor(minValue / 100) * 100;
    if (minValue > 0) {
      roundedMinValue = 0;
    }
    axisY->setRange(roundedMinValue, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 100) + 1);
    axisY->setTitleText("Traction Effort (kN)");
  } else if (m_chartTitle.contains("Dynamic Energy")) {
    maxValue =
        *std::max_element(
            m_trainSimulation->simulationDatas.energyConsumptions.begin(),
            m_trainSimulation->simulationDatas.energyConsumptions.end()) *
        1.1;
    minValue =
        *std::min_element(
            m_trainSimulation->simulationDatas.energyRegenerations.begin(),
            m_trainSimulation->simulationDatas.energyRegenerations.end()) *
        1.1;
    roundedMaxValue = ceil(maxValue / 10) * 10;
    roundedMinValue = floor(minValue / 10) * 10;
    if (minValue > 0) {
      roundedMinValue = 0;
    }
    axisY->setRange(roundedMinValue, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 10) + 1);
    axisY->setTitleText("Energy (kW)");
  } else if (m_chartTitle.contains("Distance")) {
    maxValue = *std::max_element(
        m_trainSimulation->simulationDatas.distanceTotal.begin(),
        m_trainSimulation->simulationDatas.distanceTotal.end());
    roundedMaxValue = ceil(maxValue / 1000) * 1000;
    axisY->setRange(0, roundedMaxValue);
    axisY->setTickCount(ceil(roundedMaxValue / 1000) + 1);
    axisY->setTitleText("Distance (m)");
  }
}