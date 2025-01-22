#ifndef BUTTONSIDEBAR_H
#define BUTTONSIDEBAR_H

#include <QPushButton>

class ButtonSidebar : public QPushButton {
  Q_OBJECT

public:
  explicit ButtonSidebar(const QString &iconType = "",
                         const QString &buttonLabel = "",
                         QWidget *parent = nullptr);
  using QPushButton::setText;

private:
  QPushButton *buttonSidebar;
};

#endif // BUTTONSIDEBAR_H
