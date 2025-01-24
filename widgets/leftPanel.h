#include "buttonSidebarActive.h"
#include "toggleButton.h"
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
  bool isCollapsed = false;
  ToggleButton *toggleButton;
  QList<ButtonSidebarActive *> sidebarButtons;
  QWidget *buttonContainer;
  QVBoxLayout *buttonLayout;
  void setButtonLabelsVisible(bool visible);
};
