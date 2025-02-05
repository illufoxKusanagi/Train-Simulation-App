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
  virtual ~ButtonSidebarActive() = default;
  using QPushButton::setText;
  virtual void setLabelVisible(bool visible);
  virtual void setIconVisible(bool visible);
  virtual void setEnabled(bool isEnabled);

protected:
  QString m_iconType;

private:
  static const QString s_styleHover;
  static const QString s_stylePressed;
  static const QString s_styleBase;

  QPushButton *buttonSidebarActive;
  ButtonSidebarWidget *m_contentWidget;
  // QBoxLayout *m_layout;
  QHBoxLayout *m_layout;
  bool isLabelVisible;
};

#endif // BUTTONSIDEBARACTIVE_H
