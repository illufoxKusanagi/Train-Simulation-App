#ifndef TRACTION_EFFORT_PAGE_H
#define TRACTION_EFFORT_PAGE_H

#include "controllers/simulation/train_simulation_handler.h"
#include "core/appcontext.h"
#include "resources/input_type.h"
#include "widgets/button_action.h"
#include "widgets/chart_widget.h"
#include "widgets/input_widget.h"
#include <QLabel>
#include <QMap>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

class TractionEffortPage : public QWidget {
  Q_OBJECT
public:
  explicit TractionEffortPage(QWidget *parent = nullptr,
                              TrainSimulationHandler *trainSimulation = nullptr,
                              AppContext *context = nullptr);

private:
  QVBoxLayout *mainLayout;
  QStackedWidget *stackedWidget;
  InputWidget *m_inputWidget;
  TrainSimulationHandler *m_trainSimulation;
  ButtonAction *m_prevButton;
  ButtonAction *m_nextButton;
  QMap<QString, InputWidget *> m_inputWidgets;
  QMap<QString, ChartWidget *> m_chartWidget;
  AppContext *context;

  void setupFirstPage();
  void setupSecondPage();
  void setupChart(QVBoxLayout *pageLayouts, QString chartTitle,
                  QString chartSeries);
  void setupExactValue(QVBoxLayout *pageLayout, QString inputTitle);
  void setParameterValue();
  void setStaticParameterValue();
  void setupPagination();
  void showNextPage();
  void showPreviousPage();
  void updatePaginationButtons();
};

#endif // TRACTION_EFFORT_PAGE_H
