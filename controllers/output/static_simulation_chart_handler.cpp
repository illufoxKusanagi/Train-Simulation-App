#include "static_simulation_chart_handler.h"

StaticSimulationChartHandler::StaticSimulationChartHandler(
    TrainSimulationHandler *trainSimulation, QChart *chart, QString &chartTitle,
    SaveButtonHandler::SimulationType *simulationType, AppContext *context)
    : m_trainSimulation(trainSimulation), m_chart(chart),
      m_chartTitle(chartTitle), m_simulationType(simulationType),
      m_stationData(context->stationData.data()) {}

void StaticSimulationChartHandler::setupStaticPowerChart() {
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

void StaticSimulationChartHandler::setupStaticCurrentChart() {
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

void StaticSimulationChartHandler::setupStaticSpeedChart() {
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

void StaticSimulationChartHandler::setupStaticTractionChart() {
  QLineSeries *speedSeries = new QLineSeries();
  QLineSeries *runResistanceSeries = new QLineSeries();
  QLineSeries *runResistancesZeroSeries = new QLineSeries();
  QLineSeries *runResistancesFiveSeries = new QLineSeries();
  QLineSeries *runResistancesTenSeries = new QLineSeries();
  speedSeries->setName("Static F motor");
  speedSeries->setPen(QPen(Colors::Primary500, 2));
  runResistanceSeries->setName("Static Run_Res");
  runResistanceSeries->setPen(QPen(Colors::Secondary400, 2));
  runResistancesZeroSeries->setName(
      QString("Static Run_Res %1%")
          .arg(QString::number(m_stationData->stat_slope_1)));
  runResistancesZeroSeries->setPen(QPen(Colors::Warning600, 2));
  runResistancesFiveSeries->setName(
      QString("Static Run_Res %1%")
          .arg(QString::number(m_stationData->stat_slope_2)));
  runResistancesFiveSeries->setPen(QPen(Colors::Primary700, 2));
  runResistancesTenSeries->setName(
      QString("Static Run_Res %1%")
          .arg(QString::number(m_stationData->stat_slope_3)));
  runResistancesTenSeries->setPen(QPen(Colors::Danger500, 2));
  const auto &speed = m_trainSimulation->simulationDatas.trainSpeeds;
  const auto &resistance = m_trainSimulation->simulationDatas.tractionEfforts;
  const auto &runningResistances =
      m_trainSimulation->simulationDatas.motorResistance;
  const auto &runningResistancesZero =
      m_trainSimulation->simulationDatas.motorResistancesZero;
  const auto &runningResistancesFive =
      m_trainSimulation->simulationDatas.motorResistancesFive;
  const auto &runningResistancesTen =
      m_trainSimulation->simulationDatas.motorResistancesTen;
  for (int i = 0; i < speed.size() && i < resistance.size(); ++i) {
    speedSeries->append(speed[i], resistance[i]);
    runResistanceSeries->append(speed[i], runningResistances[i]);
    runResistancesZeroSeries->append(speed[i], runningResistancesZero[i]);
    runResistancesFiveSeries->append(speed[i], runningResistancesFive[i]);
    runResistancesTenSeries->append(speed[i], runningResistancesTen[i]);
  }
  m_chart->addSeries(speedSeries);
  m_chart->addSeries(runResistanceSeries);
  qDebug() << "Adding series to chart" << m_stationData->stat_slope_1
           << m_stationData->stat_slope_2 << m_stationData->stat_slope_3;
  if (m_stationData->stat_slope_1)
    m_chart->addSeries(runResistancesZeroSeries);
  if (m_stationData->stat_slope_2)
    m_chart->addSeries(runResistancesFiveSeries);
  if (m_stationData->stat_slope_3)
    m_chart->addSeries(runResistancesTenSeries);
}

void StaticSimulationChartHandler::setupStaticEnergyChart() {
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

void StaticSimulationChartHandler::setupStaticAxis() {
  double maxValue;
  double roundedMaxValue;
  if (m_chart->series().isEmpty())
    return;
  if (*m_simulationType != SaveButtonHandler::Static)
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
    maxValue = *std::max_element(
        m_trainSimulation->simulationDatas.distanceTotal.begin(),
        m_trainSimulation->simulationDatas.distanceTotal.end());
    roundedMaxValue = ceil(maxValue / 1000) * 1000;
    axisX->setRange(0, roundedMaxValue);
    axisX->setTickCount(ceil(roundedMaxValue / 500) + 1);
    axisX->setTitleText("Distance (m)");
  } else if (m_chartTitle.contains("Static")) {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.trainSpeeds.begin(),
                   m_trainSimulation->simulationDatas.trainSpeeds.end()) *
               1.1;
    roundedMaxValue = ceil(maxValue / 10) * 10;
    axisX->setRange(0, roundedMaxValue);
    axisX->setTickCount(ceil(roundedMaxValue / 10) + 1);
    axisX->setTitleText("Speed (km/h)");
  }
  if (m_chartTitle.contains("Static Power")) {
    maxValue = *std::max_element(
                   m_trainSimulation->simulationDatas.catenaryPowers.begin(),
                   m_trainSimulation->simulationDatas.catenaryPowers.end()) *
               1.1;
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

void StaticSimulationChartHandler::updateStaticChart() {
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