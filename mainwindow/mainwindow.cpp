#include "../widgets/leftPanel.h"
#include <QMainWindow>

class MainWindow : public QMainWindow {
public:
  explicit MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
    LeftPanel *sidebar = new LeftPanel(this);
    setCentralWidget(sidebar); // Sementara sebagai widget utama
  }
};
