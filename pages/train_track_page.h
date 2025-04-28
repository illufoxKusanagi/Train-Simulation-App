#ifndef TRAIN_TRACK_PAGE_H
#define TRAIN_TRACK_PAGE_H

#include "core/appcontext.h"
#include "resources/input_type.h"
#include "widgets/button_action.h"
#include "widgets/chart_widget.h"
#include "widgets/input_widget.h"
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

class TrainTrackPage : public QWidget {
  Q_OBJECT
public:
  explicit TrainTrackPage(QWidget *parent = nullptr,
                          TrainSimulation *trainSimulation = nullptr,
                          AppContext *context = nullptr);

private:
  QVBoxLayout *mainLayout;
  TrainSimulation *m_trainSimulation;
  InputWidget *m_inputWidget;
  ButtonAction *m_prevButton;
  ButtonAction *m_nextButton;
  QStackedWidget *stackedWidget;
  QMap<QString, InputWidget *> m_inputWidgets;
  AppContext *context;

  void setupFirstPage();
  void setupSecondPage();
  void setupChart(QVBoxLayout *pageLayout, QString chartTitle);
  void setupExactValues(QVBoxLayout *pageLayout, QStringList inputTitle);
  void setParameterValue();
  void setStaticParameterValue();
  void setupPagination();
  void updatePaginationButtons();
  void showPreviousPage();
  void showNextPage();
};

#endif // TRAIN_TRACK_PAGE_H
