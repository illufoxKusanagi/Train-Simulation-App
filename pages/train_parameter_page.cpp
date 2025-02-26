#include "train_parameter_page.h"

TrainParameterPage::TrainParameterPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
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
  QList<double> values = {1.2, 4, 1.2, 24, 500, 70, 3.0, 0.0};

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
}

void TrainParameterPage::setupSecondPage(QVBoxLayout *layout) {
  const QStringList labels = {"Tc", "M1", "M2", "T1", "T2", "T3"};
  QList<double> trainValues = {2, 3, 3, 2, 1, 1};
  QList<double> passengerValues = {10, 20, 20, 10, 10, 10};
  QList<double> massValues = {100, 200, 200, 200, 200, 200};
  QWidget *numberCarContainer = new QWidget(this);
  QHBoxLayout *numberCarLayout = new QHBoxLayout(numberCarContainer);
  numberCarLayout->setSpacing(32);
  numberCarLayout->setAlignment(Qt::AlignLeft);
  numberCarLayout->setContentsMargins(16, 0, 0, 0);
  InputWidget *numberOfCar =
      new InputWidget(InputType("dropdown", "Number of Car", ""), this);
  numberCarLayout->addWidget(numberOfCar);
  layout->setSpacing(0);

  QWidget *placeholderWidget = new QWidget(this);
  placeholderWidget->setFixedSize(400, 80);
  placeholderWidget->setStyleSheet("border: 2px dashed gray; "
                                   "background-color: #f0f0f0;");
  numberCarLayout->addWidget(placeholderWidget);
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
    typeInputWidgets.append(typeInputWidget);
    typeFormLayout->addWidget(typeInputWidget);
  }
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
    massInputWidgets.append(massInputWidget);
    massFormLayout->addWidget(massInputWidget);
  }
  InputWidget *massPerTrainsetEmpty = new InputWidget(
      InputType("field", "Mass per One Trainset (empty)", "ton"), this);
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
    passangerInputWidgets.append(passengerInputWidget);
    passengerFormLayout->addWidget(passengerInputWidget);
  }
  InputWidget *massPerTrainsetFull = new InputWidget(
      InputType("field", "Mass per One Trainset(loaded)", "ton"), this);
  passengerFormLayout->addWidget(massPerTrainsetFull);
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