#include "mainwindow.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <qapplication.h>
#include <qgroupbox.h>
#include "../pages/trainParameterPage.h"
#include "../widgets/inputField.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), menuWidget(new QWidget(this)), stackedWidget(new QStackedWidget(this)) {
    // Atur layout utama
    qApp->setStyleSheet(R"(
    QWidget {
        background-color: white;
        color: black;
    }
    QGroupBox {
        border: 1px solid gray;
        border-radius: 5px;
        margin-top: 20px;
    }
    QGroupBox::title {
        subcontrol-origin: border;
        subcontrol-position: top left;
        background-color: white;
        padding: 0 5px;
        position: relative;
        left: 20px;
        top: -8px;
    }
)");


    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Setup menu kiri dan konten
    leftPanel();
    constantValuesPage();
    trainParameterPage();
    runningParameterPage();
    trackParameterPage();
    electricalParameterPage();

    // Tambahkan menu dan konten ke layout utama
    mainLayout->addWidget(menuWidget);
    mainLayout->addWidget(stackedWidget);
    mainLayout->setStretch(0, 1); // Menu 1 bagian
    mainLayout->setStretch(1, 4); // Konten 4 bagian

    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow() {}

void MainWindow::leftPanel() {
    // Layout menu
    menuWidget->setStyleSheet("background-color: #35476f;"); // Warna biru untuk panel kiri

    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);

    QStringList menuItems = {
        "Constant Value", "Train Parameter", "Running Parameter",
        "Track Parameter", "Electrical Parameter", "Export"
    };

    QStringList objectNames = {
        "constantValueButton", "trainParameterButton", "runningParameterButton",
        "trackParameterButton","electricalParameterButton","exportButton"
    };

    for (int i = 0; i < menuItems.size(); ++i) {
        QPushButton *menuButton = new QPushButton(menuItems[i], this);
        menuButton->setObjectName(objectNames[i]);
        menuButton->setStyleSheet("text-align: left; padding: 10px; font-size: 14px;"
                                  "background-color: transparent; color: white; border: none;");
        menuLayout->addWidget(menuButton);

        // Navigasi halaman
        connect(menuButton, &QPushButton::clicked, this, [this, i]() {
            navigateToPage(i);
        });
    }

    menuLayout->addStretch();

    // Set size policy for menuWidget to allow it to expand
    menuWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
}

void MainWindow::constantValuesPage() {
    // Halaman "Constant Value"
    QWidget *formPage1 = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(formPage1);

    // Group box untuk Constant Values
    QGroupBox *groupBox = new QGroupBox("Constant Values", this);
    QGridLayout *gridLayout = new QGridLayout(groupBox);

    QLabel *labelGravitation = new QLabel("Gravitation", this);
    QLineEdit *inputGravitation = new QLineEdit(this);
    inputGravitation->setText("9.8");
    inputGravitation->setReadOnly(true);
    inputGravitation->setStyleSheet("background-color: #f5f5f5;");

    QLabel *labelConversion = new QLabel("m/s to km/h", this);
    QLineEdit *inputConversion = new QLineEdit(this);
    inputConversion->setText("3.6");
    inputConversion->setReadOnly(true);
    inputConversion->setStyleSheet("background-color: #f5f5f5;");

    // Tambahkan ke grid layout
    gridLayout->addWidget(labelGravitation, 0, 0);
    gridLayout->addWidget(inputGravitation, 0, 1);
    gridLayout->addWidget(new QLabel("m/s²", this), 0, 2);

    gridLayout->addWidget(labelConversion, 1, 0);
    gridLayout->addWidget(inputConversion, 1, 1);
    gridLayout->addWidget(new QLabel("km/h", this), 1, 2);

    layout->addWidget(groupBox);

    // Tambahkan halaman ke QStackedWidget
    stackedWidget->addWidget(formPage1);
}

void MainWindow::trainParameterPage() {
    TrainParameterWidget *trainParameterWidget = new TrainParameterWidget(this);

    // Tambahkan widget custom ke QStackedWidget
    stackedWidget->addWidget(trainParameterWidget);
}

void MainWindow::runningParameterPage(){
    QWidget *formPage = new QWidget(this);
    QGridLayout *formLayout = new QGridLayout(formPage);

    QStringList labels = {"Starting Resistance", "Acceleration", "Deceleration",
                          "Difference Coasting Speed", "Weakening Point 1 (Powering)",
                          "Weakening Point 2 (Powering)", "Weakening Point 3 (Braking)",
                          "Weakening Point 4 (Braking)"};

    QStringList unitLabels = {
        "", "m/s²", "m/s²", "km/h", "km/h", "km/h","km/h","km/h"
    };

    for (int i = 0; i < labels.size(); ++i) {
        QLabel *label = new QLabel(labels[i], this);
        QLineEdit *input = new QLineEdit(this);

        // Tambahkan ke grid layout
        formLayout->addWidget(label, i / 2, (i % 2) * 2);
        formLayout->addWidget(input, i / 2, (i % 2) * 2 + 1);
        formLayout->addWidget(new QLabel(unitLabels[i], this), i / 2, (i % 2) * 2 + 1);
    }
    stackedWidget->addWidget(formPage);
}

void MainWindow::trackParameterPage(){
    QWidget *formPage = new QWidget(this);
    QGridLayout *formLayout = new QGridLayout(formPage);

    QStringList labels = {"Number of Station", "Station Distance"};

    for (int i = 0; i < labels.size(); ++i) {
        QLabel *label = new QLabel(labels[i], this);
        QLineEdit *input = new QLineEdit(this);

        // Tambahkan ke grid layout
        formLayout->addWidget(label, i / 2, (i % 2) * 2);
        formLayout->addWidget(input, i / 2, (i % 2) * 2 + 1);
    }
    stackedWidget->addWidget(formPage);
}

void MainWindow::electricalParameterPage(){
    InputField *inputField = new InputField("testing",this);

    stackedWidget->addWidget(inputField);
}

void MainWindow::navigateToPage(int pageIndex) {
    // Ganti halaman di QStackedWidget
    stackedWidget->setCurrentIndex(pageIndex);
}
