#include "train_parameter_page.h"
#include "../styles/colors.h"
#include "../styles/text_style.h"
#include "../widgets/button_action.h"
#include "../widgets/input_widget.h"
#include <QGroupBox>
#include <QPushButton>
#include <QStackedWidget>

TrainParameterPage::TrainParameterPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      stackedWidget(new QStackedWidget(this)), prevButton(nullptr),
      nextButton(nullptr) {
  groupBoxStyle = "QGroupBox { "
                  "border: 1px solid" +
                  Colors::Grey300.name() +
                  ";"
                  "padding: 16px; border-radius: 12px; " +
                  TextStyle::BodyMediumRegular() +
                  "margin-top: 20px;"
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
  mainLayout->setAlignment(Qt::AlignCenter);
  mainLayout->setContentsMargins(32, 32, 32, 32);
  setLayout(mainLayout);
  QWidget *firstPage = new QWidget(this);
  QVBoxLayout *firstPageLayout = new QVBoxLayout(firstPage);
  setupFirstPage(firstPageLayout);
  QWidget *secondPage = new QWidget(this);
  QVBoxLayout *secondPageLayout = new QVBoxLayout(secondPage);
  setupSecondPage(secondPageLayout);
  stackedWidget->addWidget(firstPage);
  stackedWidget->addWidget(secondPage);
  mainLayout->addWidget(stackedWidget);
  setupPagination();
}

void TrainParameterPage::setupFirstPage(QVBoxLayout *layout) {
  QStringList labels = {"Inertial Coefficient Trailer",
                        "Number of Axle",
                        "Inertial Coefficient Motor",
                        "Number of Traction Motor",
                        "Wheel Diameter (mm)",
                        "Passenger Weight (kg)",
                        "Gear Ratio",
                        "Load per Car (ton)"};

  QStringList unitLabels = {"", "", "", "", "mm", "kg", "", "ton"};

  QWidget *formContainer = new QWidget(this);
  QGridLayout *formLayout = new QGridLayout(formContainer);
  formContainer->setFixedSize(500, 350);
  formLayout->setAlignment(Qt::AlignCenter);
  formLayout->setContentsMargins(16, 16, 16, 16);
  formLayout->setHorizontalSpacing(64);
  formLayout->setVerticalSpacing(16);

  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget =
        new InputWidget(InputType("field", labels[i], unitLabels[i]), this);
    inputWidgets.append(inputWidget);
    formLayout->addWidget(inputWidget, i / 2, i % 2);
  }
  layout->addWidget(formContainer);
}

void TrainParameterPage::setupSecondPage(QVBoxLayout *layout) {
  const QStringList labels = {"Tc", "M1", "M2", "T1", "T2", "T3"};
  QWidget *numberCarContainer = new QWidget(this);
  QHBoxLayout *numberCarLayout = new QHBoxLayout(numberCarContainer);
  InputWidget *numberOfCar =
      new InputWidget(InputType("dropdown", "Number of Car", ""), this);
  numberCarLayout->addWidget(numberOfCar);
  QWidget *placeholderWidget = new QWidget(this);
  placeholderWidget->setFixedSize(400, 80);
  placeholderWidget->setStyleSheet("border: 2px dashed gray; "
                                   "background-color: #f0f0f0;");
  numberCarLayout->addWidget(placeholderWidget);
  layout->addWidget(numberCarContainer);
  QWidget *secondPageContainer = new QWidget(this);
  QHBoxLayout *secondPageHLayout = new QHBoxLayout(secondPageContainer);
  secondPageHLayout->setSpacing(40);
  QGroupBox *typeLayout = createTypeLayout(labels);
  secondPageHLayout->addWidget(typeLayout);
  QGroupBox *massLayout = createMassLayout(labels);
  secondPageHLayout->addWidget(massLayout);
  QGroupBox *passengerLayout = createPassengerLayout(labels);
  secondPageHLayout->addWidget(passengerLayout);
  layout->addWidget(secondPageContainer);
}

QGroupBox *TrainParameterPage::createTypeLayout(const QStringList &labels) {
  QGroupBox *typeLayout = new QGroupBox("Type");
  QVBoxLayout *typeFormLayout = new QVBoxLayout(typeLayout);
  typeFormLayout->setSpacing(0);
  for (const QString &label : labels) {
    InputWidget *typeInputWidget =
        new InputWidget(InputType("field", label, ""), this);
    typeInputWidgets.append(typeInputWidget);
    typeFormLayout->addWidget(typeInputWidget);
  }
  typeLayout->setStyleSheet(groupBoxStyle);
  return typeLayout;
}

QGroupBox *TrainParameterPage::createMassLayout(const QStringList &labels) {
  QGroupBox *massLayout = new QGroupBox("Mass");
  QVBoxLayout *massFormLayout = new QVBoxLayout(massLayout);
  massFormLayout->setSpacing(0);
  for (const QString &label : labels) {
    InputWidget *massInputWidget =
        new InputWidget(InputType("field", label, "ton"), this);
    massInputWidgets.append(massInputWidget);
    massFormLayout->addWidget(massInputWidget);
  }
  InputWidget *massPerTrainsetEmpty = new InputWidget(
      InputType("field", "Mass per One Trainset (empty)", "ton"), this);
  massFormLayout->addWidget(massPerTrainsetEmpty);

  massLayout->setStyleSheet(groupBoxStyle);
  return massLayout;
}

QGroupBox *
TrainParameterPage::createPassengerLayout(const QStringList &labels) {
  QGroupBox *passengerLayout = new QGroupBox("Passenger");
  QVBoxLayout *passengerFormLayout = new QVBoxLayout(passengerLayout);
  passengerFormLayout->setSpacing(0);
  for (const QString &label : labels) {
    InputWidget *passengerInputWidget =
        new InputWidget(InputType("field", label, "person"), this);
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
  prevButton = new ButtonAction("Constant Input", "false", this);
  nextButton = new ButtonAction("Trainset", "false", this);
  connect(prevButton, &QPushButton::clicked, this,
          &TrainParameterPage::showPreviousPage);
  connect(nextButton, &QPushButton::clicked, this,
          &TrainParameterPage::showNextPage);
  paginationLayout->addWidget(prevButton);
  paginationLayout->addWidget(nextButton);
  mainLayout->addWidget(paginationWidget);
  updatePaginationButtons();
}
void TrainParameterPage::updatePaginationButtons() {
  int currentIndex = stackedWidget->currentIndex();
  int lastIndex = stackedWidget->count() - 1;
  prevButton->setEnabled(currentIndex > 0);
  nextButton->setEnabled(currentIndex < lastIndex);
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