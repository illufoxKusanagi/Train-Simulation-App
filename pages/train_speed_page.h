#ifndef TRAIN_SPEED_PAGE_H
#define TRAIN_SPEED_PAGE_H

#include "../controllers/train_simulation.h.h"
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
  QMap<QString, InputWidget *> m_inputWidgets;
  QMap<QString, ChartWidget *> m_chartWidget;

  void setupFirstPage();
  void setupSecondPage();
  void setupChart(QVBoxLayout *pageLayout, QString chartTitle,
                  QString chartSeries);
  void setupExactValues(QVBoxLayout *pageLayout, QString inputTitle);
  void setParameterValue();
  void setStaticParameterValue();
  void setupPagination();
  void updatePaginationButtons();
  void showPreviousPage();
  void showNextPage();
  void updateCharts();
};

#endif // TRAIN_SPEED_PAGE_H