#ifndef TRACTION_EFFORT_PAGE_H
#define TRACTION_EFFORT_PAGE_H

#include "../models/train_simulation.h"
#include "../resources/input_type.h"
#include "../widgets/button_action.h"
#include "../widgets/chart_widget.h"
#include "../widgets/input_widget.h"
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

class TractionEffortPage : public QWidget {
  Q_OBJECT
public:
  explicit TractionEffortPage(QWidget *parent = nullptr,
                              TrainSimulation *trainSimulation = nullptr);

private:
  QVBoxLayout *mainLayout;
  QStackedWidget *stackedWidget;
  InputWidget *m_inputWidget;
  TrainSimulation *m_trainSimulation;
  ButtonAction *m_prevButton;
  ButtonAction *m_nextButton;
  void setupFirstPage();
  void setupSecondPage();
  void setupChart(QVBoxLayout *pageLayouts);
  void setupExactValue(QVBoxLayout *pageLayout, QString inputTitle);
  void setParameterValue();
  void setupPagination();
  void showNextPage();
  void showPreviousPage();
  void updatePaginationButtons();
};

#endif // TRACTION_EFFORT_PAGE_H