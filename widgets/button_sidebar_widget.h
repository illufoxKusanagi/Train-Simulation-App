#ifndef BUTTONSIDEBARWIDGET_H
#define BUTTONSIDEBARWIDGET_H

#include <QBoxLayout>
#include <QLabel>
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
