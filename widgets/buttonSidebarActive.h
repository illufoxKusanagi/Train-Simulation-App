#ifndef BUTTONSIDEBARACTIVE_H
#define BUTTONSIDEBARACTIVE_H

#include <QPushButton>

class ButtonSidebarActive : public QPushButton {
  Q_OBJECT

public:
  explicit ButtonSidebarActive(const QString &iconType = "",
                               const QString &buttonLabel = "",
                               QWidget *parent = nullptr);
  using QPushButton::setText;

private:
  QPushButton *buttonSidebarActive;
};

#endif // BUTTONSIDEBARACTIVE_H
