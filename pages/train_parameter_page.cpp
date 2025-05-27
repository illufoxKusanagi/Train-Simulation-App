#include "train_parameter_page.h"

TrainParameterPage::TrainParameterPage(AppContext &context,
                                       TrainSimulationHandler *trainSimulation,
                                       QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      massData(context.massData.data()), loadData(context.loadData.data()),
      trainData(context.trainData.data()), m_trainSimulation(trainSimulation),
      stackedWidget(new QStackedWidget(this)), m_prevButton(nullptr),
      m_nextButton(nullptr) {
  massHandler = new MassHandler(context);
  m_trainDataHandler = new TrainDataHandler(&context, this);
  groupBoxStyle = "QGroupBox { "
                  "border: 1px solid" +
                  Colors::Grey300.name() +
                  ";"
                  "padding: 12px 16px; border-radius: 12px; " +
                  TextStyle::BodyMediumRegular() +
                  "margin-top: 15px;"
                  "min-width: 192px;"
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
  mainLayout->setContentsMargins(8, 8, 8, 8);
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
                        "Load per Car (ton)",
                        "Load Condition (AW)",
                        "Car Length"};

  QStringList unitLabels = {"", "", "", "", "mm", "kg", "", "ton", "", "m"};
  QList<double> values = {1.05, 4, 1.1, 24, 860, 70.0, 3.0, 0.0, 0, 20};
  QStringList awOptions = {"AW4", "AW3", "AW2", "AW1", "AW0"};
  QGridLayout *formLayout = new QGridLayout(firstPageWidget);
  formLayout->setAlignment(Qt::AlignCenter);
  formLayout->setContentsMargins(16, 16, 16, 16);
  formLayout->setHorizontalSpacing(64);
  formLayout->setVerticalSpacing(32);
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget = new InputWidget(
        this,
        InputType(i == labels.count() - 2 ? "dropdown" : "field", labels[i],
                  unitLabels[i]),
        i == labels.count() - 2 ? awOptions : QStringList());
    inputWidget->setValue(values[i]);
    formLayout->addWidget(inputWidget, i / 3, i % 3);
    inputWidget->setFixedHeight(80);
    m_inputWidgets[labels[i]] = inputWidget;
  }
  setParameterValue();
  connectInputSignals();
  connectAwDataInput();
  connectTrainsetLengthInputSignal();
}

void TrainParameterPage::setupSecondPage(QVBoxLayout *layout) {
  QScrollArea *scrollArea = new QScrollArea(this);
  const QStringList labels = {"Tc", "M1", "M2",     "T1",
                              "T2", "T3", "M1-Deg", "M2-Deg"};
  const QStringList massLabels = {"Total Empty Mass", "Total Load Mass",
                                  "Total Inertial Mass"};
  QList<double> trainValues = {2, 3, 3, 2, 1, 1, 0, 0};
  QList<double> massValues = {10, 20, 20, 10, 10, 10, 20, 20};
  QList<double> passengerValues = {100, 200, 200, 200, 200, 200, 200, 200};

  setDefaultCarValues();
  QWidget *numberCarContainer = new QWidget(this);
  QHBoxLayout *numberCarLayout = new QHBoxLayout(numberCarContainer);
  numberCarLayout->setSpacing(32);
  numberCarLayout->setAlignment(Qt::AlignLeft);
  numberCarLayout->setContentsMargins(16, 0, 0, 0);

  setupTrainsetSection(numberCarLayout, m_carData);

  layout->setSpacing(0);
  layout->addWidget(numberCarContainer);
  layout->setAlignment(Qt::AlignCenter);
  QWidget *secondPageContainer = new QWidget(this);
  QHBoxLayout *secondPageHLayout = new QHBoxLayout(secondPageContainer);
  secondPageHLayout->setSpacing(8);
  QGroupBox *typeLayout = createTypeLayout(labels, trainValues);
  secondPageHLayout->addWidget(typeLayout);
  QGroupBox *massLayout = createMassLayout(labels, massValues);
  secondPageHLayout->addWidget(massLayout);
  QGroupBox *passengerLayout = createPassengerLayout(labels, passengerValues);
  secondPageHLayout->addWidget(passengerLayout);
  QGroupBox *outputMassLayout = createOutputMassLayout(massLabels);
  secondPageHLayout->addWidget(outputMassLayout);
  connectTypeInputSignals();
  connectMassInputSignals();
  connectPassengerInputSignals();
  layout->addWidget(secondPageContainer);
}

QGroupBox *TrainParameterPage::createTypeLayout(const QStringList &labels,
                                                QList<double> values) {
  QGroupBox *typeLayout = new QGroupBox("Type");
  QVBoxLayout *groupBoxMainLayout = new QVBoxLayout(typeLayout);
  QScrollArea *scrollArea = new QScrollArea(typeLayout);
  scrollArea->setWidgetResizable(false);
  scrollArea->setFrameShape(QFrame::NoFrame);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QWidget *scrollWidget = new QWidget();
  QVBoxLayout *typeFormLayout = new QVBoxLayout(scrollWidget);
  for (const QString &label : labels) {
    InputWidget *typeInputWidget =
        new InputWidget(this, InputType("field", label, ""));
    typeInputWidget->setValue(values[labels.indexOf(label)]);
    typeFormLayout->addWidget(typeInputWidget);
    m_typeInputWidgets[label] = typeInputWidget;
  }
  scrollArea->setWidget(scrollWidget);
  groupBoxMainLayout->addWidget(scrollArea);
  setTypeValue();
  typeLayout->setStyleSheet(groupBoxStyle);
  return typeLayout;
}

QGroupBox *TrainParameterPage::createMassLayout(const QStringList &labels,
                                                QList<double> values) {
  QGroupBox *massLayout = new QGroupBox("Mass");
  QVBoxLayout *mainMassFormLayout = new QVBoxLayout(massLayout);
  QScrollArea *scrollArea = new QScrollArea(massLayout);
  scrollArea->setWidgetResizable(false);
  scrollArea->setFrameShape(QFrame::NoFrame);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QWidget *scrollWidget = new QWidget();
  QVBoxLayout *massFormLayout = new QVBoxLayout(scrollWidget);
  massFormLayout->setAlignment(Qt::AlignTop);
  for (const QString &label : labels) {
    InputWidget *massInputWidget =
        new InputWidget(this, InputType("field", label, "ton"));
    massInputWidget->setValue(values[labels.indexOf(label)]);
    m_massInputWidgets[label] = massInputWidget;
    massFormLayout->addWidget(massInputWidget);
  }
  scrollArea->setWidget(scrollWidget);
  mainMassFormLayout->addWidget(scrollArea);
  setMassValue();
  massLayout->setStyleSheet(groupBoxStyle);
  return massLayout;
}

QGroupBox *TrainParameterPage::createPassengerLayout(const QStringList &labels,
                                                     QList<double> values) {
  QGroupBox *passengerLayout = new QGroupBox("Passenger");
  QVBoxLayout *mainPassengerFormLayout = new QVBoxLayout(passengerLayout);
  QScrollArea *scrollArea = new QScrollArea(passengerLayout);
  scrollArea->setWidgetResizable(false);
  scrollArea->setFrameShape(QFrame::NoFrame);
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QWidget *scrollWidget = new QWidget();
  QVBoxLayout *passengerFormLayout = new QVBoxLayout(scrollWidget);
  for (const QString &label : labels) {
    InputWidget *passengerInputWidget =
        new InputWidget(this, InputType("field", label, "person"));
    passengerInputWidget->setValue(values[labels.indexOf(label)]);
    passengerFormLayout->addWidget(passengerInputWidget);
    m_passengerInputWidgets[label] = passengerInputWidget;
  }
  scrollArea->setWidget(scrollWidget);
  mainPassengerFormLayout->addWidget(scrollArea);
  setPassengerValue();
  passengerLayout->setStyleSheet(groupBoxStyle);
  return passengerLayout;
}

QGroupBox *
TrainParameterPage::createOutputMassLayout(const QStringList &labels) {
  QGroupBox *outputMassLayout = new QGroupBox("Output Mass");
  QVBoxLayout *outputMassFormLayout = new QVBoxLayout(outputMassLayout);
  outputMassLayout->setAlignment(Qt::AlignTop);
  massPerTrainsetEmpty = new InputWidget(
      this, InputType("field", "Mass per One Trainset (empty)", "ton", true));
  massPerTrainsetEmpty->setValue(calculateEmptyMass());
  massPerTrainsetLoaded = new InputWidget(
      this, InputType("field", "Mass per One Trainset (loaded)", "ton", true));
  massPerTrainsetLoaded->setValue(calculateLoadedMass());
  massPerTrainsetInertial = new InputWidget(
      this, InputType("field", "Inertial Mass per One Trainset", "ton", true));
  massPerTrainsetInertial->setValue(calculateInertialMass());
  outputMassFormLayout->addWidget(massPerTrainsetEmpty);
  outputMassFormLayout->addWidget(massPerTrainsetLoaded);
  outputMassFormLayout->addWidget(massPerTrainsetInertial);
  outputMassLayout->setStyleSheet(groupBoxStyle);
  return outputMassLayout;
}

void TrainParameterPage::setupPagination() {
  QWidget *paginationWidget = new QWidget(this);
  QHBoxLayout *paginationLayout = new QHBoxLayout(paginationWidget);
  paginationLayout->setAlignment(Qt::AlignLeft);
  paginationLayout->setSpacing(16);
  m_prevButton = new ButtonAction(this, "Constant Input");
  m_nextButton = new ButtonAction(this, "Trainset");
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
  m_trainDataHandler->storeFormInputs(m_inputWidgets);
  // trainData->n_tm = getParameterValue("Number of Traction Motor");
  // trainData->n_axle = getParameterValue("Number of Axle");
  // trainData->wheel = getParameterValue("Wheel Diameter (mm)");
  // trainData->gearRatio = getParameterValue("Gear Ratio");
  // loadData->load = getParameterValue("Load per Car (ton)");
  // loadData->mass_P = getParameterValue("Passenger Weight (kg)");
  // massData->i_M = getParameterValue("Inertial Coefficient Motor");
  // massData->i_T = getParameterValue("Inertial Coefficient Trailer");
}

void TrainParameterPage::setTypeValue() {
  trainData->n_Tc = getTypeParameterValue("Tc");
  trainData->n_M1 = getTypeParameterValue("M1");
  trainData->n_M2 = getTypeParameterValue("M2");
  trainData->n_T1 = getTypeParameterValue("T1");
  trainData->n_T2 = getTypeParameterValue("T2");
  trainData->n_T3 = getTypeParameterValue("T3");
  setDegradedTrainMotorValues(getTypeParameterValue("M1-Deg"),
                              getTypeParameterValue("M2-Deg"));
}

void TrainParameterPage::setMassValue() {
  massData->mass_M1 = getMassParameterValue("M1");
  massData->mass_M2 = getMassParameterValue("M2");
  massData->mass_TC = getMassParameterValue("Tc");
  massData->mass_T1 = getMassParameterValue("T1");
  massData->mass_T2 = getMassParameterValue("T2");
  massData->mass_T3 = getMassParameterValue("T3");
}

void TrainParameterPage::setPassengerValue() {
  loadData->n_PM1 = getPassengerParameterValue("M1");
  loadData->n_PM2 = getPassengerParameterValue("M2");
  loadData->n_PTc = getPassengerParameterValue("Tc");
  loadData->n_PT1 = getPassengerParameterValue("T1");
  loadData->n_PT2 = getPassengerParameterValue("T2");
  loadData->n_PT3 = getPassengerParameterValue("T3");
}

void TrainParameterPage::connectInputSignals() {
  for (auto it = m_inputWidgets.constBegin(); it != m_inputWidgets.constEnd();
       ++it) {
    QString paramName = it.key();
    InputWidget *widget = it.value();
    connect(it.value(), &InputWidget::valueChanged, this, [this, paramName]() {
      setParameterValue();
      updateMassCalculation();
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

void TrainParameterPage::connectOutputMassInputSignals() {
  for (auto it = m_outputMassInputWidgets.constBegin();
       it != m_outputMassInputWidgets.constEnd(); ++it) {
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
  double inertialMass = calculateInertialMass();
  massPerTrainsetInertial->setValue(inertialMass);
}

double TrainParameterPage::calculateEmptyMass() {
  massData->mass_totalEmpty = massHandler->countMassEmptyCar();
  return massData->mass_totalEmpty;
}

double TrainParameterPage::calculateLoadedMass() {
  massData->mass_totalLoad = (loadData->load > 0)
                                 ? massHandler->countMassLoadInput()
                                 : massHandler->countMassWithLoad();
  return massData->mass_totalLoad;
}

double TrainParameterPage::calculateInertialMass() {
  double result;
  if (loadData->load > 0) {
    result = massHandler->countInertialMassInput();
  } else {
    result = massHandler->countInertialMass();
  }
  massData->mass_totalInertial = result;
  return result;
}

void TrainParameterPage::updateTrainImage(QLabel *trainImageLabel, int nCar) {
  QString filename = QString(":/icons/icons/%1-train.png").arg(nCar);
  QPixmap pixmap(filename);
  trainImageLabel->setPixmap(pixmap.scaled(
      trainImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void TrainParameterPage::setupTrainsetSection(
    QHBoxLayout *numberCarLayout, QList<QList<QList<double>>> carData) {
  QStringList nCarOptions = {"12", "10", "8", "6", "12-Degraded"};
  m_numberOfCar = new InputWidget(this, InputType("dropdown", "Number of Car"),
                                  nCarOptions);
  m_trainLabelImage = new QLabel(this);
  m_numberOfCar->setValue(12);
  trainData->n_car = 12;
  m_trainLabelImage->setFixedSize(512, 80);
  updateTrainImage(m_trainLabelImage, 12);
  connect(m_numberOfCar, &InputWidget::valueChanged, this, [this, carData] {
    double value = m_numberOfCar->getValue();
    int index = -1;
    const QStringList labels = {"Tc", "M1", "M2",     "T1",
                                "T2", "T3", "M1-Deg", "M2-Deg"};
    trainData->n_car = index;
    if (value == 0) {
      index = 12;
      trainData->n_M1_disabled = 0;
      trainData->n_M2_disabled = 0;
    } else if (value == 1) {
      index = 10;
      trainData->n_M1_disabled = 0;
      trainData->n_M2_disabled = 0;
    } else if (value == 2) {
      index = 8;
      trainData->n_M1_disabled = 0;
      trainData->n_M2_disabled = 0;
    } else if (value == 3) {
      index = 6;
      trainData->n_M1_disabled = 0;
      trainData->n_M2_disabled = 0;
    } else if (value == 4) {
      index = 14;
      trainData->n_car = 12;
      trainData->n_M1_disabled = 1;
      trainData->n_M2_disabled = 1;
    }
    updateTrainImage(m_trainLabelImage, index);
    if (value >= 0 && value < carData[0].size()) {
      for (int i = 0; i < labels.size(); i++) {
        if (m_typeInputWidgets.contains(labels[i])) {
          m_typeInputWidgets[labels[i]]->setValue(carData[0][value][i]);
          m_massInputWidgets[labels[i]]->setValue(carData[1][value][i]);
        }
      }
    }
    updateMassCalculation();
    updateTrainsetLengthValue();
    emit trainsetChanged();
  });
  numberCarLayout->addWidget(m_numberOfCar);
  numberCarLayout->addWidget(m_trainLabelImage);
  updateTrainsetLengthValue();
}

void TrainParameterPage::connectAwDataInput() {
  connect(m_inputWidgets["Load Condition (AW)"], &InputWidget::valueChanged,
          this, [this]() {
            int index = static_cast<int>(
                m_inputWidgets["Load Condition (AW)"]->getValue());
            const QStringList labels = {"Tc", "M1", "M2",     "T1",
                                        "T2", "T3", "M1-Deg", "M2-Deg"};
            for (int i = 0; i < labels.size(); i++) {
              if (m_passengerInputWidgets.contains(labels[i])) {
                m_passengerInputWidgets[labels[i]]->setValue(
                    m_carData[2][index][i]);
              }
            }
            awDataChanged();
          });
}

void TrainParameterPage::setDefaultCarValues() {
  QList<double> twlv_typeTrainValues = {2, 3, 3, 2, 1, 1, 0, 0};
  QList<double> ten_typeTrainValues = {2, 3, 3, 2, 0, 0, 0, 0};
  QList<double> eght_typeTrainValues = {2, 2, 2, 1, 1, 0, 0, 0};
  QList<double> six_typeTrainValues = {2, 1, 1, 2, 0, 0, 0, 0};
  QList<double> twlvDeg_typeTrainValues = {2, 2, 2, 2, 1, 1, 1, 1};

  QList<double> twlv_massTrainValues = {10, 20, 20, 10, 10, 10, 0, 0};
  QList<double> ten_massTrainValues = {10, 20, 20, 10, 0, 0, 0, 0};
  QList<double> eght_massTrainValues = {10, 20, 20, 10, 10, 0, 0, 0};
  QList<double> six_massTrainValues = {10, 20, 20, 10, 0, 0, 0, 0};
  QList<double> twlvDeg_massTrainValues = {10, 20, 20, 10, 10, 10, 20, 20};

  QList<double> twlv_passTrainValues = {100, 200, 200, 200, 200, 200, 0, 0};
  QList<double> ten_passTrainValues = {100, 200, 200, 200, 0, 0, 0, 0};
  QList<double> eght_passTrainValues = {100, 200, 200, 200, 200, 0, 0, 0};
  QList<double> six_passTrainValues = {100, 200, 200, 200, 0, 0, 0, 0};
  QList<double> twlvDeg_passTrainValues = {100, 200, 200, 200,
                                           200, 200, 200, 200};

  QList<double> aw0_passTrainValues = {0, 0, 0, 0, 0, 0, 0, 0};
  QList<double> aw1_passTrainValues = {
      10, 20, 20, 20, 20, 20,
  };
  QList<double> aw2_passTrainValues = {20, 40, 40, 40, 40, 40, 40, 40};
  QList<double> aw3_passTrainValues = {50, 100, 100, 100, 100, 100, 100, 100};
  QList<double> aw4_passTrainValues = {100, 200, 200, 200, 200, 200, 200, 200};

  QList<QList<double>> m_trainValues = {
      twlv_typeTrainValues, ten_typeTrainValues, eght_typeTrainValues,
      six_typeTrainValues, twlvDeg_typeTrainValues};
  QList<QList<double>> m_trainMasses = {
      twlv_massTrainValues, ten_massTrainValues, eght_massTrainValues,
      six_massTrainValues, twlvDeg_massTrainValues};
  QList<QList<double>> m_trainPassengers = {
      aw4_passTrainValues, aw3_passTrainValues, aw2_passTrainValues,
      aw1_passTrainValues, aw0_passTrainValues, twlvDeg_passTrainValues};
  m_carData = {m_trainValues, m_trainMasses, m_trainPassengers};
}

double TrainParameterPage::getAwData() {
  double awIndex = getParameterValue("Load Condition (AW)");
  return awIndex;
}

void TrainParameterPage::connectTrainsetLengthInputSignal() {
  connect(m_inputWidgets["Car Length"], &InputWidget::valueChanged, this,
          &TrainParameterPage::updateTrainsetLengthValue);
}

void TrainParameterPage::updateTrainsetLengthValue() {
  trainData->trainsetLength =
      (trainData->n_car * getParameterValue("Car Length"));
}

void TrainParameterPage::setDegradedTrainMotorValues(double numberM1,
                                                     double numberM2) {
  trainData->n_M1_disabled = numberM1;
  trainData->n_M2_disabled = numberM2;
}