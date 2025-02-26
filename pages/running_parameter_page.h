#ifndef RUNNINGPARAMETERWIDGET_H
#define RUNNINGPARAMETERWIDGET_H

#include "../styles/colors.h"
#include "../styles/text_style.h"
#include "../widgets/input_widget.h"
#include <QGridLayout>
#include <QWidget>

class RunningParameterPage : public QWidget {
  Q_OBJECT
public:
  explicit RunningParameterPage(QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
  QWidget *m_formLayout;
  QGridLayout *m_inputsLayout;
  QMap<QString, InputWidget *> m_inputWidgets;
  void createInputs();
};

#endif // RUNNINGPARAMETERWIDGET_H
