#ifndef TRACK_PARAMETER_PAGE_H
#define TRACK_PARAMETER_PAGE_H

#include "controllers/train_simulation/train_simulation.h"
#include "core/appcontext.h"
#include "models/moving_data.h"
#include "models/resistance_data.h"
#include "models/station_data.h"
#include "models/train_data.h"
#include "widgets/input_widget.h"
#include <QGridLayout>
#include <QWidget>

class TrackParameterPage : public QWidget {
  Q_OBJECT
public:
  explicit TrackParameterPage(AppContext &context,
                              TrainSimulation *trainSimulation = nullptr,
                              QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
  QWidget *m_formContainer;
  QGridLayout *m_formLayout;
  QMap<QString, InputWidget *> m_inputWidgets;
  ResistanceData *resistanceData;
  MovingData *movingData;
  StationData *stationData;
  TrainSimulation *m_trainSimulation;

  void createInputs();
  double getParameterValue(const QString &paramName) const;
  QList<double> getCsvParamValue(const QString &paramName,
                                 const int requiredColumn) const;
  void setParameterValue();
  void connectInputSignals();
  void setSlopeValue();
  void setRadiusValue();
  void setMaxSpeedValue();
  void setStationDistanceValue();
};

#endif // TRACK_PARAMETER_PAGE_H
