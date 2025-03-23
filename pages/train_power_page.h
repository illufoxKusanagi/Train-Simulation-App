#ifndef TRAIN_POWER_PAGE_H
#define TRAIN_POWER_PAGE_H

#include "../models/train_simulation.h"
#include "../resources/input_type.h"
#include "../widgets/button_action.h"
#include "../widgets/chart_widget.h"
#include "../widgets/input_widget.h"
#include <QLabel>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

class TrainPowerPage : public QWidget {
  Q_OBJECT
public:
  explicit TrainPowerPage(QWidget *parentWidget = nullptr,
                          TrainSimulation *trainSimulation = nullptr);
  void updateCharts();
private slots:
  void pageChanged(int pageIndex);
  void updatePageButtons();

private:
  QVBoxLayout *mainLayout;
  TrainSimulation *m_trainSimulation;
  ButtonAction *m_prevButton;
  ButtonAction *m_nextButton;
  ButtonAction *m_firstPageButton;
  ButtonAction *m_secondPageButton;
  ButtonAction *m_thirdPageButton;
  ButtonAction *m_fourthPageButton;
  QStackedWidget *stackedWidget;
  QMap<QString, InputWidget *> m_inputWidgets;
  QMap<QString, ChartWidget *> m_chartWidgets;

  void setupFirstPage();
  void setupSecondPage();
  void setupThirdPage();
  void setupFourthPage();
  void setupChart(QVBoxLayout *pageLayout, QString chartTitle,
                  QString chartSeries);
  void setupInputs(QVBoxLayout *pageLayout, QStringList inputTitle,
                   QStringList inputUnit);
  double getParameterValue(const QString &paramName) const;
  void setParameterValue();
  void setStaticParameterValue();
  void setupPagination();
  void showNextPage();
  void showPreviousPage();
  void updatePaginationButtons();
};

#endif // TRAIN_POWER_PAGE_H