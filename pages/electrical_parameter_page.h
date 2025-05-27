#ifndef ELECTRICAL_PARAMETER_PAGE_H
#define ELECTRICAL_PARAMETER_PAGE_H

#include "controllers/data/electrical_data_handler.h"
#include "controllers/simulation/train_simulation_handler.h"
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
                                   TrainSimulationHandler *trainSimulation,
                                   QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
  QWidget *m_formContainer;
  QGridLayout *m_formLayout;
  QMap<QString, InputWidget *> m_inputWidgets;
  EfficiencyData *efficiencyData;
  PowerData *powerData;
  EnergyData *energyData;
  TrainSimulationHandler *m_trainSimulation;
  ElectricalDataHandler *m_electricalDataHandler;

  void createInputs();
  // double getParameterValue(const QString &paramName) const;
  // QList<double> getCsvParamValue(const QString &paramName,
  //                                const int requiredColumn) const;
  void setParameterValue();
  void connectInputSignals();
};

#endif // ELECTRICAL_PARAMETER_PAGE_H
