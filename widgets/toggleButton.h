#ifndef TOGGLEBUTTON_H
#define TOGGLEBUTTON_H

#include <QPushButton>
#include <QWidget>

class ToggleButton : public QPushButton {
  Q_OBJECT
public:
  explicit ToggleButton(bool isCollapsed = false, QWidget *parent = nullptr);
  void updateIcon();
  void toggleCollapse();

private:
  bool m_isCollapsed = false;
};

#endif // TOGGLEBUTTON_H
