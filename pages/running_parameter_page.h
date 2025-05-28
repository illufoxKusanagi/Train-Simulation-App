#ifndef RUNNINGPARAMETERWIDGET_H
#define RUNNINGPARAMETERWIDGET_H

#include "controllers/data/running_data_handler.h"
#include "controllers/simulation/train_simulation_handler.h"
#include "core/appcontext.h"
#include "models/train_data.h"
#include "styles/colors.h"
#include "styles/text_style.h"
#include "train_parameter_page.h"
#include "widgets/input_widget.h"
#include <QGridLayout>
#include <QMap>
#include <QWidget>

class RunningParameterPage : public QWidget {
  Q_OBJECT

public slots:
  void onAwChanged(double awIndex);

public:
  explicit RunningParameterPage(AppContext &context,
                                TrainParameterPage *trainParameterPage,
                                QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
  QWidget *m_formLayout;
  QGridLayout *m_inputsLayout;
  QMap<QString, InputWidget *> m_inputWidgets;
  MovingData *movingData;
  ResistanceData *resistanceData;
  LoadData *loadData;
  TrainData *trainData;
  TrainParameterPage *m_trainParameterPage;
  RunningDataHandler *m_runningDataHandler;

  void createInputs();
  double getParameterValue(const QString &paramName) const;
  void setParameterValue();
  void connectInputSignals();
  void setAccelerationValue();
  void setDecelerationValue();
};

#endif // RUNNINGPARAMETERWIDGET_H
