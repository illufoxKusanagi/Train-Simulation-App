#ifndef TRAIN_SPEED_PAGE_H
#define TRAIN_SPEED_PAGE_H

#include "../models/train_simulation.h"
#include "../resources/input_type.h"
#include "../widgets/button_action.h"
#include "../widgets/chart_widget.h"
#include "../widgets/input_widget.h"
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

class TrainSpeedPage : public QWidget {
  Q_OBJECT
public:
  explicit TrainSpeedPage(QWidget *parent = nullptr,
                          TrainSimulation *trainSimulation = nullptr);

private:
  QVBoxLayout *mainLayout;
  TrainSimulation *m_trainSimulation;
  InputWidget *m_inputWidget;
  ButtonAction *m_prevButton;
  ButtonAction *m_nextButton;
  QStackedWidget *stackedWidget;

  void setupFirstPage();
  void setupSecondPage();
  void setupChart(QVBoxLayout *pageLayout);
  void setupExactValues(QVBoxLayout *pageLayout, QString inputTitle);
  void setParameterValue();
  void setupPagination();
  void updatePaginationButtons();
  void showPreviousPage();
  void showNextPage();
};

#endif // TRAIN_SPEED_PAGE_H