#ifndef ELECTRICAL_PARAMETER_PAGE_H
#define ELECTRICAL_PARAMETER_PAGE_H

#include "models/efficiency_data.h"
#include "models/energy_data.h"
#include "models/power_data.h"
#include "models/train_data.h"
#include "widgets/input_widget.h"
#include <QGridLayout>
#include <QMap>
#include <QWidget>

class ElectricalParameterPage : public QWidget {
  Q_OBJECT
public:
  explicit ElectricalParameterPage(QWidget *parent = nullptr,
                                   EfficiencyData *efficiencyData = nullptr,
                                   PowerData *powerData = nullptr,
                                   EnergyData *energyData = nullptr);

private:
  QVBoxLayout *mainLayout;
  QWidget *m_formContainer;
  QGridLayout *m_formLayout;
  QMap<QString, InputWidget *> m_inputWidgets;
  EfficiencyData *efficiencyData;
  PowerData *powerData;
  EnergyData *energyData;

  void createInputs();
  double getParameterValue(const QString &paramName) const;
  void setParameterValue();
  void connectInputSignals();
};

#endif // ELECTRICAL_PARAMETER_PAGE_H
