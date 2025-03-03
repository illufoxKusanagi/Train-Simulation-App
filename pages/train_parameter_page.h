#ifndef TRAINPARAMETERPAGE_H
#define TRAINPARAMETERPAGE_H

#include "../models/train_data.h"
#include "../models/train_simulation.h"
#include "../styles/colors.h"
#include "../styles/text_style.h"
#include "../widgets/button_action.h"
#include "../widgets/input_widget.h"
#include <QGroupBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

class TrainParameterPage : public QWidget {
  Q_OBJECT

public:
  explicit TrainParameterPage(QWidget *parent = nullptr,
                              TrainData *trainData = nullptr,
                              MassData *massData = nullptr,
                              LoadData *loadData = nullptr,
                              TrainSimulation *trainSimulation = nullptr);

private:
  QVBoxLayout *mainLayout;
  QStackedWidget *stackedWidget;
  QMap<QString, InputWidget *> m_inputWidgets;
  QMap<QString, InputWidget *> m_typeInputWidgets;
  QMap<QString, InputWidget *> m_massInputWidgets;
  QMap<QString, InputWidget *> m_passengerInputWidgets;
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
  TrainSimulation *m_trainSimulation;
  QLabel *m_trainLabelImage;
  InputWidget *massPerTrainsetEmpty;
  InputWidget *massPerTrainsetLoaded;

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
  void updateMassCalculation();
  double calculateEmptyMass();
  double calculateLoadedMass();

  QGroupBox *createTypeLayout(const QStringList &labels, QList<double> values);
  QGroupBox *createMassLayout(const QStringList &labels, QList<double> values);
  QGroupBox *createPassengerLayout(const QStringList &labels,
                                   QList<double> values);
};

#endif // TRAINPARAMETERPAGE_H