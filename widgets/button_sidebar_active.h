#ifndef BUTTONSIDEBARACTIVE_H
#define BUTTONSIDEBARACTIVE_H

#include "../resources/icon_paths.h"
#include "../styles/colors.h"
#include "../styles/text_style.h"
#include "button_sidebar_widget.h"
#include <QBoxLayout>
#include <QPushButton>

class ButtonSidebarActive : public QPushButton {
  Q_OBJECT

public:
  explicit ButtonSidebarActive(const QString &iconType = "",
                               const QString &buttonLabel = "",
                               QWidget *parent = nullptr);
  using QPushButton::setText;
  void setLabelVisible(bool visible);
  void setEnabled(bool isEnabled);

private:
  static const QString s_styleHover;
  static const QString s_stylePressed;
  static const QString s_styleBase;

  QPushButton *buttonSidebarActive;
  ButtonSidebarWidget *m_contentWidget;
  QString m_iconType;
  QString m_iconPath;
  bool isLabelVisible;
  void updateIcon(QString iconType);
};

#endif // BUTTONSIDEBARACTIVE_H
