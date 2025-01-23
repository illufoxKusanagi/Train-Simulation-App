#include "mainwindow.h"
#include "../resources/inputType.h"
#include "../widgets/buttonAction.h"
#include "../widgets/buttonSidebarActive.h"
#include <QVBoxLayout>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
  centralWidget->setLayout(mainLayout);

  InputType inputType("input", "Mass per One Trainset", "ton");
  InputType dropdownType("dropdown", "Number of Trainsets");
  InputType uploadType("upload", "Import Trainset Data");
  InputType invalidType("hahaha", "Testing", "fjsa");

  // InputWidget *inputWidget = new InputWidget(inputType, this);
  // mainLayout->addWidget(inputWidget);

  // InputWidget *dropdownWidget = new InputWidget(dropdownType, this);
  // mainLayout->addWidget(dropdownWidget);

  // InputWidget *invalidWidget = new InputWidget(invalidType,this);
  // mainLayout->addWidget(invalidWidget);

  // InputWidget *uploadWidget = new InputWidget(uploadType, this);

  // ButtonAction *saveButton = new ButtonAction("Save this data", "", this);
  // connect(saveButton, &ButtonAction::clicked, this,
  //         [this]() { qDebug() << "Submit button clicked"; });
  ButtonAction *runButton = new ButtonAction("Run", "icon", this);
  connect(runButton, &ButtonAction::clicked, this,
          [this]() { qDebug() << "Run button clicked"; });
  // ButtonAction *saveFileButton = new ButtonAction("Save all data", "", this);
  // connect(saveFileButton, &ButtonAction::clicked, this,
  //         [this]() { qDebug() << "Save all data button clicked"; });
  // mainLayout->addWidget(saveButton);
  mainLayout->addWidget(runButton);
  // mainLayout->addWidget(saveFileButton);

  ButtonSidebarActive *constantValuesButton =
      new ButtonSidebarActive("runningActive", "Constant Values", this);
  ButtonSidebarActive *trainParameterButton =
      new ButtonSidebarActive("trainActive", "Train Parameter", this);
  ButtonSidebarActive *runningParameterButton =
      new ButtonSidebarActive("runningActive", "Running Parameter", this);
  ButtonSidebarActive *trackParameterButton =
      new ButtonSidebarActive("trackActive", "Track Parameter", this);
  ButtonSidebarActive *electricalParameterButton =
      new ButtonSidebarActive("electricalActive", "Electrical Parameter", this);
  ButtonSidebarActive *outputButton =
      new ButtonSidebarActive("outputActive", "Output", this);
  mainLayout->addWidget(constantValuesButton);
  mainLayout->addWidget(trainParameterButton);
  mainLayout->addWidget(runningParameterButton);
  mainLayout->addWidget(trackParameterButton);
  mainLayout->addWidget(electricalParameterButton);
  mainLayout->addWidget(outputButton);
}

MainWindow::~MainWindow() {}

void MainWindow::navigateToPage(int pageIndex) {
  stackedWidget->setCurrentIndex(pageIndex);
}
