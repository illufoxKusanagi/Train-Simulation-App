#include "track_parameter_page.h"

TrackParameterPage::TrackParameterPage(AppContext &context,
                                       TrainSimulationHandler *trainSimulation,
                                       QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      m_formContainer(new QWidget(this)),
      m_formLayout(new QGridLayout(m_formContainer)),
      resistanceData(context.resistanceData.data()),
      movingData(context.movingData.data()),
      stationData(context.stationData.data()),
      m_trainSimulation(trainSimulation) {
  mainLayout->setContentsMargins(16, 16, 16, 16);
  mainLayout->setAlignment(Qt::AlignCenter);
  m_formLayout->setHorizontalSpacing(128);
  m_formLayout->setVerticalSpacing(32);
  m_trackDataHandler = new TrackDataHandler(&context, this);
  createInputs();
  connect(m_trainSimulation, &TrainSimulationHandler::simulationStarted, this,
          [this]() { setParameterValue(); });
  mainLayout->addWidget(m_formContainer);
  setLayout(mainLayout);
}

void TrackParameterPage::createInputs() {
  QStringList labels = {"Number of Station", "Radius per Section",
                        "Station Distance", "Slope per Section", "Speed Limit"};
  QStringList units = {"", "m", "m", "‰", "km/h"};
  QStringList types = {"field", "field upload", "field upload", "field upload",
                       "field upload"};
  QList<double> values = {2, 2000, 2000, 0.0, 70};
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(this, InputType(types[i], labels[i], units[i]));
    inputWidget->setValue(values[i]);
    inputWidget->setMinimumWidth(80);
    m_formLayout->addWidget(inputWidget, i / 2, i % 2);
    m_inputWidgets[labels[i]] = inputWidget;
  }
  setParameterValue();
  connectInputSignals();
}

double TrackParameterPage::getParameterValue(const QString &paramName) const {
  if (m_inputWidgets.contains(paramName)) {
    return m_inputWidgets[paramName]->getValue();
  }
  return 0.0;
}

QList<double>
TrackParameterPage::getCsvParamValue(const QString &paramName,
                                     const int requiredColumn) const {
  if (!m_inputWidgets.isEmpty()) {
    return m_inputWidgets[paramName]->getCsvValue(requiredColumn);
  }
  return QList<double>();
}

void TrackParameterPage::setParameterValue() {
  m_trackDataHandler->storeFormInputs(m_inputWidgets);
}

void TrackParameterPage::connectInputSignals() {
  for (auto it = m_inputWidgets.constBegin(); it != m_inputWidgets.constEnd();
       ++it) {
    QString paramName = it.key();
    InputWidget *widget = it.value();
    connect(it.value(), &InputWidget::valueChanged, this, [this, paramName]() {
      setParameterValue();
      double value = getParameterValue(paramName);
    });
  }
}

// void TrackParameterPage::setStationDistanceValue() {
//   stationData->stat_x_station = getParameterValue("Station Distance");
//   QList<double> stationDistances = getCsvParamValue("Station Distance", 2);
//   stationData->x_station =
//       std::vector<double>(stationDistances.begin(), stationDistances.end());
//   QList<double> totalStationDistances = getCsvParamValue("Station Distance",
//   1); stationData->tot_x_station = std::vector<double>(
//       totalStationDistances.begin(), totalStationDistances.end());
// }

// void TrackParameterPage::setSlopeValue() {
//   QString paramName = "Slope per Section";
//   stationData->stat_slope = getParameterValue(paramName);
//   QList<double> slopes = getCsvParamValue(paramName, 2);
//   stationData->slope = std::vector<double>(slopes.begin(), slopes.end());
//   QList<double> slopeStartDistances = getCsvParamValue(paramName, 0);
//   stationData->x_slopeStart =
//   std::vector<double>(slopeStartDistances.begin(),
//                                                   slopeStartDistances.end());
//   QList<double> slopeEndDistances = getCsvParamValue(paramName, 1);
//   stationData->x_slopeEnd =
//       std::vector<double>(slopeEndDistances.begin(),
//       slopeEndDistances.end());
// }

// void TrackParameterPage::setRadiusValue() {
//   QString paramName = "Radius per Section";
//   stationData->stat_radius = getParameterValue(paramName);
//   QList<double> radiuses = getCsvParamValue(paramName, 2);
//   stationData->radius = std::vector<double>(radiuses.begin(),
//   radiuses.end()); QList<double> radiusStartDistances =
//   getCsvParamValue(paramName, 0); stationData->x_radiusStart =
//   std::vector<double>(radiusStartDistances.begin(),
//                                                    radiusStartDistances.end());
//   QList<double> radiusEndDistances = getCsvParamValue(paramName, 1);
//   stationData->x_radiusEnd =
//       std::vector<double>(radiusEndDistances.begin(),
//       radiusEndDistances.end());
// }

// void TrackParameterPage::setMaxSpeedValue() {
//   QString paramName = "Speed Limit";
//   stationData->stat_v_limit = getParameterValue(paramName);
//   QList<double> maxSpeeds = getCsvParamValue(paramName, 2);
//   stationData->v_limit =
//       std::vector<double>(maxSpeeds.begin(), maxSpeeds.end());
//   QList<double> maxSpeedStartDistances = getCsvParamValue(paramName, 0);
//   stationData->x_v_limitStart = std::vector<double>(
//       maxSpeedStartDistances.begin(), maxSpeedStartDistances.end());
//   QList<double> maxSpeedEndDistances = getCsvParamValue(paramName, 1);
//   stationData->x_v_limitEnd =
//   std::vector<double>(maxSpeedEndDistances.begin(),
//                                                   maxSpeedEndDistances.end());
// }