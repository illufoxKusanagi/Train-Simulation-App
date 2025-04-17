#ifndef BUTTON_PANEL_CATEGORY_H
#define BUTTON_PANEL_CATEGORY_H

#include "resources/icon_paths.h"
#include "styles/colors.h"
#include "styles/text_style.h"
#include <QBoxLayout>
#include <QLabel>
#include <QPushButton>

class ButtonPanelCategory : public QPushButton {
  Q_OBJECT
public:
  explicit ButtonPanelCategory(QWidget *parent = nullptr,
                               const QString &iconType = "",
                               const QString &buttonLabel = "");
  void setActive(bool isActive);

private:
  static const QString s_styleBase;
  static const QString s_styleHover;
  static const QString s_stylePressed;
  static const QString s_styleInactive;

  QPushButton *m_buttonPanelCategory;
  QBoxLayout *mainLayout;
  QString m_buttonLabel;
  QString m_iconType;
  QLabel *m_textLabel;
  QLabel *m_iconLabel;
  bool m_isActive = false;

  void setTextIcon();
  void updateIcon(bool isActive);
};

#endif // BUTTON_PANEL_CATEGORY_H
