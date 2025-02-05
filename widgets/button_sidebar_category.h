#ifndef BUTTON_SIDEBAR_CATEGORY_H
#define BUTTON_SIDEBAR_CATEGORY_H

#include "button_sidebar_active.h"

class ButtonSidebarCategory : public ButtonSidebarActive {
  Q_OBJECT
public:
  explicit ButtonSidebarCategory(const QString &iconType = "",
                                 const QString &buttonLabel = "",
                                 QWidget *parent = nullptr);
  void setActive(bool isCollapse, bool isActive);
};

#endif // BUTTON_SIDEBAR_CATEGORY_H
