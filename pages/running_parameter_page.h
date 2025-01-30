#ifndef RUNNINGPARAMETERWIDGET_H
#define RUNNINGPARAMETERWIDGET_H

#include "../widgets/button_action.h"
#include "../widgets/input_widget.h"
#include <QGridLayout>
#include <QWidget>


class RunningParameterWidget : public QWidget {
  Q_OBJECT
public:
  explicit RunningParameterWidget(QWidget *parent = nullptr);

private:
  QGridLayout *formLayout;
};

#endif // RUNNINGPARAMETERWIDGET_H
