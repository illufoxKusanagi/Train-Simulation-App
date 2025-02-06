#ifndef LEFT_PANEL_H
#define LEFT_PANEL_H

#include "button_sidebar_active.h"
#include "button_toggle.h"
#include "left_panel_category.h"
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
  bool m_isCollapsed = false;
  int m_currentIndex = -1;
  ButtonToggle *buttonToggle;
  QVBoxLayout *buttonLayout;
  QList<ButtonSidebarActive *> sidebarButtons;
  LeftPanelCategory *m_inputPanel;
  LeftPanelCategory *m_outputPanel;

  int getCurrentIndex() const { return m_currentIndex; }
  void setCurrentIndex(int index) { m_currentIndex = index; }
  // void setupButtons();
  // void updateButtonStates();
  void setupInputPageButtons();
  void setupOutputPageButtons();

};

#endif // LEFT_PANEL_H
