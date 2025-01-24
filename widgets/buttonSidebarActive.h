#ifndef BUTTONSIDEBARACTIVE_H
#define BUTTONSIDEBARACTIVE_H

#include "buttonSidebarWidget.h"
#include <QPushButton>


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
  ButtonSidebarWidget *m_contentWidget;
  bool isLabelVisible;
};

#endif // BUTTONSIDEBARACTIVE_H
