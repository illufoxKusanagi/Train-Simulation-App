#include "train_parameter_page.h"

TrainParameterPage::TrainParameterPage(QWidget *parent, TrainData *trainData,
                                       MassData *massData, LoadData *loadData,
                                       TrainSimulation *trainSimulation)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)), massData(massData),
      loadData(loadData), trainData(trainData),
      m_trainSimulation(trainSimulation),
      stackedWidget(new QStackedWidget(this)), m_prevButton(nullptr),
      m_nextButton(nullptr) {
  groupBoxStyle = "QGroupBox { "
                  "border: 1px solid" +
                  Colors::Grey300.name() +
                  ";"
                  "padding: 12px 16px; border-radius: 12px; " +
                  TextStyle::BodyMediumRegular() +
                  "margin-top: 15px;"
                  "min-width: 200px;"
                  "}"
                  "QGroupBox::title {"
                  "subcontrol-origin: border;"
                  "subcontrol-position: top left;"
                  "background-color: white;"
                  "padding: 0 5px;"
                  "position: relative;"
                  "left: 20px;"
                  "top: -8px;"
                  "}";
  setStyleSheet("position: relative;");
  mainLayout->setAlignment(Qt::AlignVCenter);
  mainLayout->setContentsMargins(32, 16, 32, 16);
  mainLayout->setSpacing(0);
  setLayout(mainLayout);
  QWidget *firstPageWidget = new QWidget(this);
  setupFirstPage(firstPageWidget);
  QWidget *secondPage = new QWidget(this);
  QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPage);
  setupSecondPage(secondPageLayout);
  stackedWidget->addWidget(firstPageWidget);
  stackedWidget->addWidget(secondPage);
  mainLayout->addWidget(stackedWidget);
  setupPagination();
}

void TrainParameterPage::setupFirstPage(QWidget *firstPageWidget) {
  QStringList labels = {"Inertial Coefficient Trailer",
                        "Number of Axle",
                        "Inertial Coefficient Motor",
                        "Number of Traction Motor",
                        "Wheel Diameter (mm)",
                        "Passenger Weight (kg)",
                        "Gear Ratio",
                        "Load per Car (ton)"};

  QStringList unitLabels = {"", "", "", "", "mm", "kg", "", "ton"};
  QList<double> values = {1.2, 4, 1.2, 24, 500, 70.0, 3.0, 0.0};

  QGridLayout *formLayout = new QGridLayout(firstPageWidget);
  formLayout->setAlignment(Qt::AlignCenter);
  formLayout->setContentsMargins(16, 16, 16, 16);
  formLayout->setHorizontalSpacing(128);
  formLayout->setVerticalSpacing(32);
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("field", labels[i], unitLabels[i]), this);
    inputWidget->setValue(values[i]);
    formLayout->addWidget(inputWidget, i / 2, i % 2);
    m_inputWidgets[labels[i]] = inputWidget;
  }
  setParameterValue();
  connectInputSignals();
}

void TrainParameterPage::setupSecondPage(QVBoxLayout *layout) {
  const QStringList labels = {"Tc", "M1", "M2", "T1", "T2", "T3"};
  QList<double> trainValues = {2, 3, 3, 2, 1, 1};
  QList<double> massValues = {10, 20, 20, 10, 10, 10};
  QList<double> passengerValues = {100, 200, 200, 200, 200, 200};
  QWidget *numberCarContainer = new QWidget(this);
  QHBoxLayout *numberCarLayout = new QHBoxLayout(numberCarContainer);
  numberCarLayout->setSpacing(32);
  numberCarLayout->setAlignment(Qt::AlignLeft);
  numberCarLayout->setContentsMargins(16, 0, 0, 0);
  layout->setSpacing(0);

  m_numberOfCar =
      new InputWidget(InputType("dropdown", "Number of Car", ""), this);
  m_numberOfCar->setValue(12);
  trainData->n_car = 12;

  numberCarLayout->addWidget(m_numberOfCar);

  m_trainLabelImage = new QLabel(this);
  m_trainLabelImage->setFixedSize(512, 80);
  updateTrainImage(m_trainLabelImage, 12);
  connect(m_numberOfCar, &InputWidget::valueChanged, this, [this] {
    double value = m_numberOfCar->getValue();
    trainData->n_car = value;
    updateTrainImage(m_trainLabelImage, value);
    updateMassCalculation();
  });
  numberCarLayout->addWidget(m_trainLabelImage);

  layout->addWidget(numberCarContainer);

  QWidget *secondPageContainer = new QWidget(this);
  QHBoxLayout *secondPageHLayout = new QHBoxLayout(secondPageContainer);
  secondPageHLayout->setSpacing(40);
  QGroupBox *typeLayout = createTypeLayout(labels, trainValues);
  secondPageHLayout->addWidget(typeLayout);
  QGroupBox *massLayout = createMassLayout(labels, massValues);
  secondPageHLayout->addWidget(massLayout);
  QGroupBox *passengerLayout = createPassengerLayout(labels, passengerValues);
  secondPageHLayout->addWidget(passengerLayout);
  connectTypeInputSignals();
  connectMassInputSignals();
  connectPassengerInputSignals();
  layout->addWidget(secondPageContainer);
}

QGroupBox *TrainParameterPage::createTypeLayout(const QStringList &labels,
                                                QList<double> values) {
  QGroupBox *typeLayout = new QGroupBox("Type");
  QVBoxLayout *typeFormLayout = new QVBoxLayout(typeLayout);
  typeFormLayout->setAlignment(Qt::AlignTop);
  for (const QString &label : labels) {
    InputWidget *typeInputWidget =
        new InputWidget(InputType("field", label, ""), this);
    typeInputWidget->setValue(values[labels.indexOf(label)]);
    typeFormLayout->addWidget(typeInputWidget);
    m_typeInputWidgets[label] = typeInputWidget;
  }
  setTypeValue();
  typeLayout->setStyleSheet(groupBoxStyle);
  return typeLayout;
}

QGroupBox *TrainParameterPage::createMassLayout(const QStringList &labels,
                                                QList<double> values) {
  QGroupBox *massLayout = new QGroupBox("Mass");
  QVBoxLayout *massFormLayout = new QVBoxLayout(massLayout);
  for (const QString &label : labels) {
    InputWidget *massInputWidget =
        new InputWidget(InputType("field", label, "ton"), this);
    massInputWidget->setValue(values[labels.indexOf(label)]);
    m_massInputWidgets[label] = massInputWidget;
    massFormLayout->addWidget(massInputWidget);
  }
  setMassValue();
  massPerTrainsetEmpty = new InputWidget(
      InputType("field", "Mass per One Trainset (empty)", "ton", 0, true),
      this);
  massPerTrainsetEmpty->setValue(90.0);
  massFormLayout->addWidget(massPerTrainsetEmpty);
  massLayout->setStyleSheet(groupBoxStyle);
  return massLayout;
}

QGroupBox *TrainParameterPage::createPassengerLayout(const QStringList &labels,
                                                     QList<double> values) {
  QGroupBox *passengerLayout = new QGroupBox("Passenger");
  QVBoxLayout *passengerFormLayout = new QVBoxLayout(passengerLayout);
  for (const QString &label : labels) {
    InputWidget *passengerInputWidget =
        new InputWidget(InputType("field", label, "person"), this);
    passengerInputWidget->setValue(values[labels.indexOf(label)]);
    passengerFormLayout->addWidget(passengerInputWidget);
    m_passengerInputWidgets[label] = passengerInputWidget;
  }
  setPassengerValue();
  massPerTrainsetLoaded = new InputWidget(
      InputType("field", "Mass per One Trainset(loaded)", "ton", 0, true),
      this);
  passengerFormLayout->addWidget(massPerTrainsetLoaded);
  passengerLayout->setStyleSheet(groupBoxStyle);
  return passengerLayout;
}

void TrainParameterPage::setupPagination() {
  QWidget *paginationWidget = new QWidget(this);
  QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
  paginationLayout->setAlignment(Qt::AlignLeft);
  paginationLayout->setSpacing(16);
  m_prevButton = new ButtonAction("Constant Input", "false", this);
  m_nextButton = new ButtonAction("Trainset", "false", this);
  m_prevButton->setFixedSize(144, 48);
  m_nextButton->setFixedSize(144, 48);
  connect(m_prevButton, &QPushButton::clicked, this,
          &TrainParameterPage::showPreviousPage);
  connect(m_nextButton, &QPushButton::clicked, this,
          &TrainParameterPage::showNextPage);
  paginationLayout->addWidget(m_prevButton);
  paginationLayout->addWidget(m_nextButton);
  mainLayout->addWidget(paginationWidget);
  updatePaginationButtons();
}

void TrainParameterPage::updatePaginationButtons() {
  int currentIndex = stackedWidget->currentIndex();
  int lastIndex = stackedWidget->count() - 1;
  m_prevButton->setEnabled(currentIndex > 0);
  m_nextButton->setEnabled(currentIndex < lastIndex);
}

void TrainParameterPage::showPreviousPage() {
  if (stackedWidget->currentIndex() > 0) {
    stackedWidget->setCurrentIndex(stackedWidget->currentIndex() - 1);
    updatePaginationButtons();
  }
}

void TrainParameterPage::showNextPage() {
  if (stackedWidget->currentIndex() < stackedWidget->count() - 1) {
    stackedWidget->setCurrentIndex(stackedWidget->currentIndex() + 1);
    updatePaginationButtons();
  }
}

double TrainParameterPage::getParameterValue(const QString &paramName) {
  if (m_inputWidgets.contains(paramName)) {
    return m_inputWidgets[paramName]->getValue();
  }
  return 0.0;
}

double TrainParameterPage::getTypeParameterValue(const QString &paramName) {
  if (m_typeInputWidgets.contains(paramName)) {
    return m_typeInputWidgets[paramName]->getValue();
  }
  return 0.0;
}

double TrainParameterPage::getMassParameterValue(const QString &paramName) {
  if (m_massInputWidgets.contains(paramName)) {
    return m_massInputWidgets[paramName]->getValue();
  }
  return 0.0;
}

double
TrainParameterPage::getPassengerParameterValue(const QString &paramName) {
  if (m_passengerInputWidgets.contains(paramName)) {
    return m_passengerInputWidgets[paramName]->getValue();
  }
  return 0.0;
}

void TrainParameterPage::setParameterValue() {
  trainData->n_tm = getParameterValue("Number of Traction Motor");
  trainData->n_axle = getParameterValue("Number of Axle");
  trainData->wheel = getParameterValue("Wheel Diameter (mm)");
  trainData->gearRatio = getParameterValue("Gear Ratio");
  loadData->load = getParameterValue("Load per Car (ton)");
  loadData->m_P = getParameterValue("Passenger Weight (kg)");
  massData->i_M = getParameterValue("Inertial Coefficient Motor");
  massData->i_T = getParameterValue("Inertial Coefficient Trailer");
  // qDebug() << "Current data values:";
  // qDebug() << "  Number of Traction Motor:" << trainData->n_tm;
  // qDebug() << "  Number of Axle:" << trainData->n_axle;
  // qDebug() << "  Wheel Diameter:" << trainData->wheel;
  // qDebug() << "  Gear Ratio:" << trainData->gearRatio;
  // qDebug() << "  Load per Car:" << loadData->load;
  // qDebug() << "  Passenger Weight:" << loadData->m_P;
  // qDebug() << "  Inertial Coefficient Motor:" << massData->i_M;
  // qDebug() << "  Inertial Coefficient Trailer:" << massData->i_T;
}

void TrainParameterPage::setTypeValue() {
  loadData->n_Tc = getTypeParameterValue("Tc");
  loadData->n_M1 = getTypeParameterValue("M1");
  loadData->n_M2 = getTypeParameterValue("M2");
  loadData->n_T1 = getTypeParameterValue("T1");
  loadData->n_T2 = getTypeParameterValue("T2");
  loadData->n_T3 = getTypeParameterValue("T3");
  // qDebug() << "Current data values:";
  // qDebug() << "  Tc:" << loadData->n_Tc;
  // qDebug() << "  M1:" << loadData->n_M1;
  // qDebug() << "  M2:" << loadData->n_M2;
  // qDebug() << "  T1:" << loadData->n_T1;
  // qDebug() << "  T2:" << loadData->n_T2;
  // qDebug() << "  T3:" << loadData->n_T3;
}

void TrainParameterPage::setMassValue() {
  massData->m_M1 = getMassParameterValue("M1");
  massData->m_M2 = getMassParameterValue("M2");
  massData->m_TC = getMassParameterValue("Tc");
  massData->m_T1 = getMassParameterValue("T1");
  massData->m_T2 = getMassParameterValue("T2");
  massData->m_T3 = getMassParameterValue("T3");
  // qDebug() << "Current data values:";
  // qDebug() << "  M1:" << massData->m_M1;
  // qDebug() << "  M2:" << massData->m_M2;
  // qDebug() << "  Tc:" << massData->m_TC;
  // qDebug() << "  T1:" << massData->m_T1;
  // qDebug() << "  T2:" << massData->m_T2;
  // qDebug() << "  T3:" << massData->m_T3;
}

void TrainParameterPage::setPassengerValue() {
  loadData->n_PM1 = getPassengerParameterValue("M1");
  loadData->n_PM2 = getPassengerParameterValue("M2");
  loadData->n_PTc = getPassengerParameterValue("Tc");
  loadData->n_PT1 = getPassengerParameterValue("T1");
  loadData->n_PT2 = getPassengerParameterValue("T2");
  loadData->n_PT3 = getPassengerParameterValue("T3");
  // qDebug() << "Current data values:";
  // qDebug() << "  M1:" << loadData->n_PM1;
  // qDebug() << "  M2:" << loadData->n_PM2;
  // qDebug() << "  Tc:" << loadData->n_PTc;
  // qDebug() << "  T1:" << loadData->n_PT1;
  // qDebug() << "  T2:" << loadData->n_PT2;
  // qDebug() << "  T3:" << loadData->n_PT3;
}

void TrainParameterPage::connectInputSignals() {
  for (auto it = m_inputWidgets.constBegin(); it != m_inputWidgets.constEnd();
       ++it) {
    QString paramName = it.key();
    InputWidget *widget = it.value();
    connect(it.value(), &InputWidget::valueChanged, this, [this, paramName]() {
      setParameterValue();
      updateMassCalculation();
      // qDebug() << "Parameter" << paramName << "changed to:" << value;

      // // Additional debug information
      // qDebug() << "Current data values:";
      // qDebug() << "  Number of Traction Motor:" << trainData->n_tm;
      // qDebug() << "  Number of Axle:" << trainData->n_axle;
      // qDebug() << "  Wheel Diameter:" << trainData->wheel;
      // qDebug() << "  Gear Ratio:" << trainData->gearRatio;
      // qDebug() << "  Load per Car:" << loadData->load;
      // qDebug() << "  Passenger Weight:" << loadData->m_P;
      // qDebug() << "  Inertial Coefficient Motor:" << massData->i_M;
      // qDebug() << "  Inertial Coefficient Trailer:" << massData->i_T;
    });
  }
}

void TrainParameterPage::connectTypeInputSignals() {
  for (auto it = m_typeInputWidgets.constBegin();
       it != m_typeInputWidgets.constEnd(); ++it) {
    QString paramName = it.key();
    InputWidget *widget = it.value();
    connect(it.value(), &InputWidget::valueChanged, this, [this, paramName]() {
      setTypeValue();
      updateMassCalculation();
      // qDebug() << "Parameter" << paramName << "changed to:" << value;

      // // Additional debug information
      // qDebug() << "Current data values:";
      // qDebug() << "  Tc:" << loadData->n_Tc;
      // qDebug() << "  M1:" << loadData->n_M1;
      // qDebug() << "  M2:" << loadData->n_M2;
      // qDebug() << "  T1:" << loadData->n_T1;
      // qDebug() << "  T2:" << loadData->n_T2;
      // qDebug() << "  T3:" << loadData->n_T3;
    });
  }
}

void TrainParameterPage::connectMassInputSignals() {
  for (auto it = m_massInputWidgets.constBegin();
       it != m_massInputWidgets.constEnd(); ++it) {
    QString paramName = it.key();
    InputWidget *widget = it.value();
    connect(it.value(), &InputWidget::valueChanged, this, [this, paramName]() {
      setMassValue();
      updateMassCalculation();
      // qDebug() << "Parameter" << paramName << "changed to:" << value;

      // // Additional debug information
      // qDebug() << "Current data values:";
      // qDebug() << "  M1:" << massData->m_M1;
      // qDebug() << "  M2:" << massData->m_M2;
      // qDebug() << "  Tc:" << massData->m_TC;
      // qDebug() << "  T1:" << massData->m_T1;
      // qDebug() << "  T2:" << massData->m_T2;
      // qDebug() << "  T3:" << massData->m_T3;
    });
  }
}

void TrainParameterPage::connectPassengerInputSignals() {
  for (auto it = m_passengerInputWidgets.constBegin();
       it != m_passengerInputWidgets.constEnd(); ++it) {
    QString paramName = it.key();
    InputWidget *widget = it.value();
    connect(it.value(), &InputWidget::valueChanged, this, [this, paramName]() {
      setPassengerValue();
      updateMassCalculation();
    });
  }
}

void TrainParameterPage::updateMassCalculation() {
  double emptyMass = calculateEmptyMass();
  massPerTrainsetEmpty->setValue(emptyMass);
  double loadedMass = calculateLoadedMass();
  massPerTrainsetLoaded->setValue(loadedMass);
}

double TrainParameterPage::calculateEmptyMass() {
  massData->m_totalEmpty = m_trainSimulation->countMassEmptyCar();
  return massData->m_totalEmpty;
}

double TrainParameterPage::calculateLoadedMass() {
  massData->m_totalLoad = (loadData->load > 0)
                              ? m_trainSimulation->countMassLoadInput()
                              : m_trainSimulation->countMassWithLoad();
  return massData->m_totalLoad;
}

void TrainParameterPage::updateTrainImage(QLabel *trainImageLabel, int nCar) {
  QString filename = QString(":/icons/icons/%1-train.png").arg(nCar);
  QPixmap pixmap(filename);
  trainImageLabel->setPixmap(pixmap.scaled(
      trainImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}