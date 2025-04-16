#ifndef TRACK_PARAMETER_PAGE_H
#define TRACK_PARAMETER_PAGE_H

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
  explicit TrackParameterPage(QWidget *parent = nullptr,
                              MovingData *movingData = nullptr,
                              ResistanceData *resistanceData = nullptr,
                              StationData *stationData = nullptr);

private:
  QVBoxLayout *mainLayout;
  QWidget *m_formContainer;
  QGridLayout *m_formLayout;
  QMap<QString, InputWidget *> m_inputWidgets;
  ResistanceData *resistanceData;
  MovingData *movingData;
  StationData *stationData;

  void createInputs();
  double getParameterValue(const QString &paramName) const;
  QList<double> getCsvParamValue(const QString &paramName) const;
  void setParameterValue();
  void connectInputSignals();
};

#endif // TRACK_PARAMETER_PAGE_H
