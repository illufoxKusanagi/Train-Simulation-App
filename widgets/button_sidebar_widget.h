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
  void updateIcon(const QString &IconPath);

private:
  QLabel *iconLabel = nullptr;
  QLabel *textLabel = nullptr;
  QBoxLayout *layout = nullptr;
};

#endif // BUTTONSIDEBARWIDGET_H
