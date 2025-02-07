#ifndef BUTTONSIDEBARWIDGET_H
#define BUTTONSIDEBARWIDGET_H

#include "../resources/icon_paths.h"
#include "../styles/colors.h"
#include "../styles/text_style.h"
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QSizePolicy>
#include <Qwidget>


class ButtonSidebarWidget : public QWidget {
public:
  explicit ButtonSidebarWidget(const QString &iconType,
                               const QString &buttonLabel,
                               QWidget *parent = nullptr);
  void setLabelVisible(bool visible);

  void setIconVisible(bool visible);
  void updateIcon(bool isEnabled);
  void setEnabled(bool isEnabled);
  void updateAppearance(bool isShown);

private:
  QLabel *m_iconLabel = nullptr;
  QLabel *m_textLabel = nullptr;
  QBoxLayout *mainLayout = nullptr;
  QString m_iconType;
  bool m_isEnabled = false;
};

#endif // BUTTONSIDEBARWIDGET_H
