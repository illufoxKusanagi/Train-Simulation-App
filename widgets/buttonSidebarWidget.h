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

private:
  QLabel *iconLabel;
  QLabel *buttonLabel;
  QBoxLayout *layout;
};

#endif // BUTTONSIDEBARWIDGET_H
