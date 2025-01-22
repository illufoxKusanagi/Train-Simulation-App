#include "mainwindow.h"
#include "../widgets/InputWidget.h"
#include "../widgets/buttonAction.h"
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

  ButtonAction *saveButton = new ButtonAction("Save this data", "" , this);
  connect(saveButton, &ButtonAction::clicked, this,
          [this]() { qDebug() << "Submit button clicked"; });
  ButtonAction *runButton = new ButtonAction("Run Simulation", "icon", this);
  connect(runButton, &ButtonAction::clicked, this,
          [this]() { qDebug() << "Run button clicked"; });
  ButtonAction *saveFileButton = new ButtonAction("Save all data", "", this);
  connect(saveFileButton, &ButtonAction::clicked, this,
          [this]() { qDebug() << "Save all data button clicked"; });
  mainLayout->addWidget(saveButton);
  mainLayout->addWidget(runButton);
  mainLayout->addWidget(saveFileButton);
}

MainWindow::~MainWindow() {}

void MainWindow::navigateToPage(int pageIndex) {
  stackedWidget->setCurrentIndex(pageIndex);
}
