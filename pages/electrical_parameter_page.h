#ifndef ELECTRICAL_PARAMETER_PAGE_H
#define ELECTRICAL_PARAMETER_PAGE_H

#include "core/appcontext.h"
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
  explicit ElectricalParameterPage(AppContext &context,
                                   QWidget *parent = nullptr);

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
  QList<double> getCsvParamValue(const QString &paramName,
                                 const int requiredColumn) const;
  void setGearboxEfficiencyValue();
  void setVvvfEfficiencyValue();
  void setTractionMotorValue();
  void setLineVoltageValue();
  void setMotorVoltageValue();
  void setPowerFactorValue();
};

#endif // ELECTRICAL_PARAMETER_PAGE_H
