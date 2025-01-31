#ifndef RUNNINGPARAMETERWIDGET_H
#define RUNNINGPARAMETERWIDGET_H

#include "../widgets/button_action.h"
#include "../widgets/input_widget.h"
#include <QGridLayout>
#include <QWidget>

class RunningParameterPage : public QWidget {
  Q_OBJECT
public:
  explicit RunningParameterPage(QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
  QWidget *formLayout;
  QGridLayout *inputsLayout;
  void createInputs();
};

#endif // RUNNINGPARAMETERWIDGET_H
