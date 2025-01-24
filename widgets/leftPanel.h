#ifndef LEFT_PANEL_H
#define LEFT_PANEL_H

#include "buttonSidebarActive.h"
#include "toggleButton.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>


class LeftPanel : public QWidget {
  Q_OBJECT

public:
  explicit LeftPanel(QWidget *parent = nullptr);

signals:
  void navigateToPage(int pageIndex);

private:
  bool isCollapsed = false;
  ToggleButton *toggleButton;
  QVBoxLayout *buttonLayout;
  QList<ButtonSidebarActive *> sidebarButtons;

  void setupButtons();
  void emitNavigateSignal(int pageIndex);
};

#endif // LEFT_PANEL_H