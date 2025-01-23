#ifndef BUTTONSIDEBARACTIVE_H
#define BUTTONSIDEBARACTIVE_H

#include <QPushButton>
#include "buttonSidebarWidget.h"

class ButtonSidebarActive : public QPushButton {
  Q_OBJECT

public:
  explicit ButtonSidebarActive(const QString &iconType = "",
                               const QString &buttonLabel = "",
                               QWidget *parent = nullptr);
  using QPushButton::setText;
  void setLabelVisible(bool visible);

private:
  QPushButton *buttonSidebarActive;
  ButtonSidebarWidget *contentWidget;
  bool isLabelVisible;
};

#endif // BUTTONSIDEBARACTIVE_H
