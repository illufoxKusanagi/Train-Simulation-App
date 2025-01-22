#include "mainwindow.h"
#include "../widgets/InputWidget.h"
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

  InputWidget *uploadWidget = new InputWidget(uploadType, this);
}

MainWindow::~MainWindow() {}

void MainWindow::navigateToPage(int pageIndex) {
  stackedWidget->setCurrentIndex(pageIndex);
}
