#ifndef LEFT_PANEL_INPUTS_H
#define LEFT_PANEL_INPUTS_H

#include "button_panel_category.h"
#include "button_sidebar_active.h"
#include "button_sidebar_category.cpp"
#include <QVBoxLayout>
#include <QWidget>

class LeftPanelInputs : public QWidget {
  Q_OBJECT
public:
  enum PanelType { INPUT, OUTPUT };
  explicit LeftPanelInputs(PanelType type, QWidget *parent = nullptr);
  void toggleCollapse(bool isCollapsed);

public slots:
  void onPageChanged(int pageIndex);

signals:
  void buttonClicked(int index);

private:
  static const QStringList INPUT_BUTTON_NAMES;
  static const QStringList INPUT_BUTTON_TYPES;
  static const QStringList OUTPUT_BUTTON_NAMES;
  static const QStringList OUTPUT_BUTTON_TYPES;

  int m_currentIndex = 0;
  const QStringList m_buttonNames;
  const QStringList m_buttonTypes;
  ButtonSidebarCategory *m_categoryLabel;
  QVBoxLayout *buttonLayout;
  QList<ButtonSidebarActive *> m_sidebarButtons;
  PanelType m_type;
  void setupButtons();
  void updateButtonStates();
};

#endif // LEFT_PANEL_INPUTS_H
