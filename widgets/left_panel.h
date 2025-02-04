#ifndef LEFT_PANEL_H
#define LEFT_PANEL_H

#include "../styles/colors.h"
#include "../styles/text_style.h"
#include "button_sidebar_active.h"
#include "button_toggle.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class LeftPanel : public QWidget {
  Q_OBJECT

public:
  explicit LeftPanel(QWidget *parent = nullptr);

public slots:
  // void onPageChanged(int pageIndex);
  void emitNavigateSignal(int pageIndex);

signals:
  void navigateToPage(int pageIndex);

private:
  bool isCollapsed = false;
  ButtonToggle *buttonToggle;
  QVBoxLayout *buttonLayout;
  QList<ButtonSidebarActive *> sidebarButtons;

  void setupButtons();
  void setSidebarButtonsEnabled(bool enable);
};

#endif // LEFT_PANEL_H