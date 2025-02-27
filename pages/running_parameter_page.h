#ifndef RUNNINGPARAMETERWIDGET_H
#define RUNNINGPARAMETERWIDGET_H

#include "../models/train_data.h"
#include "../models/train_simulation.h"
#include "../styles/colors.h"
#include "../styles/text_style.h"
#include "../widgets/input_widget.h"
#include <QGridLayout>
#include <QMap>
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
  MovingData movingData;
  ResistanceData resistanceData;

  void createInputs();
  double getParameterValue(const QString &paramName) const;
  void setParameterValue();
  void connectInputSignals();
};

#endif // RUNNINGPARAMETERWIDGET_H
