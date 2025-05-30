#ifndef TRAINPARAMETERPAGE_H
#define TRAINPARAMETERPAGE_H

#include "controllers/data/train_data_handler.h"
#include "controllers/simulation/mass_handler.h"
#include "controllers/simulation/train_simulation_handler.h"
#include "core/appcontext.h"
#include "models/train_data.h"
#include "styles/colors.h"
#include "styles/text_style.h"
#include "widgets/button_action.h"
#include "widgets/input_widget.h"
#include <QGroupBox>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

class TrainParameterPage : public QWidget {
  Q_OBJECT

signals:
  void awDataChanged();
  void trainsetChanged();

public:
  explicit TrainParameterPage(AppContext &context,
                              TrainSimulationHandler *trainSimulation,
                              QWidget *parent = nullptr);
  double getAwData();

private:
  QVBoxLayout *mainLayout;
  QStackedWidget *stackedWidget;
  QMap<QString, InputWidget *> m_inputWidgets;
  QMap<QString, InputWidget *> m_typeInputWidgets;
  QMap<QString, InputWidget *> m_massInputWidgets;
  QMap<QString, InputWidget *> m_passengerInputWidgets;
  QMap<QString, InputWidget *> m_outputMassInputWidgets;
  QList<InputWidget *> inputWidgets;
  QList<InputWidget *> typeInputWidgets;
  QList<InputWidget *> massInputWidgets;
  QList<InputWidget *> passangerInputWidgets;
  QString groupBoxStyle;
  ButtonAction *m_prevButton;
  ButtonAction *m_nextButton;
  InputWidget *m_numberOfCar;
  TrainData *trainData;
  MassData *massData;
  LoadData *loadData;
  MassHandler *massHandler;
  TrainSimulationHandler *m_trainSimulation;
  QLabel *m_trainLabelImage;
  InputWidget *massPerTrainsetEmpty;
  InputWidget *massPerTrainsetLoaded;
  InputWidget *massPerTrainsetInertial;
  QList<QList<QList<double>>> m_carData;
  TrainDataHandler *m_trainDataHandler;

  void setupFirstPage(QWidget *firstPageWidget);
  void setupSecondPage(QVBoxLayout *layout);
  void setupPagination();
  void showPreviousPage();
  void showNextPage();
  void updatePaginationButtons();
  double getParameterValue(const QString &paramName);
  double getTypeParameterValue(const QString &paramName);
  double getMassParameterValue(const QString &paramName);
  double getPassengerParameterValue(const QString &paramName);
  void setParameterValue();
  void setTypeValue();
  void setMassValue();
  void setPassengerValue();
  void connectInputSignals();
  void connectTypeInputSignals();
  void connectMassInputSignals();
  void connectPassengerInputSignals();
  void connectOutputMassInputSignals();
  void connectAwDataInput();
  void updateMassCalculation();
  double calculateEmptyMass();
  double calculateLoadedMass();
  double calculateInertialMass();
  void updateTrainImage(QLabel *trainImageLabel, int nCar);
  void setupTrainsetSection(QHBoxLayout *numberCarLayout,
                            QList<QList<QList<double>>> carData);
  void connectTrainsetLengthInputSignal();
  void setDefaultCarValues();
  void updateTrainsetLengthValue();
  void setDegradedTrainMotorValues(double numberM1, double numberM2);

  QGroupBox *createTypeLayout(const QStringList &labels, QList<double> values);
  QGroupBox *createMassLayout(const QStringList &labels, QList<double> values);
  QGroupBox *createPassengerLayout(const QStringList &labels,
                                   QList<double> values);
  QGroupBox *createOutputMassLayout(const QStringList &labels);
};

#endif // TRAINPARAMETERPAGE_H
