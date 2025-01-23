#include "buttonSidebarActive.h"
#include <QList>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class LeftPanel : public QWidget {
public:
  explicit LeftPanel(QWidget *parent = nullptr);

private slots:
  void togglePanel();

private:
  bool isCollapsed;
  QPushButton *toggleButton;
  QList<ButtonSidebarActive *> sidebarButtons;
  void setButtonLabelsVisible(bool visible);
};
