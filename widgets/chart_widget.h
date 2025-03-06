#ifndef CHART_WIDGET_H
#define CHART_WIDGET_H

#include "../models/train_data.h"
#include "../models/train_simulation.h"
#include "../widgets/button_action.h"
#include "../widgets/button_pagination.h"
#include <QFileDialog>
#include <QFont>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

class ChartWidget : public QWidget {
  Q_OBJECT
public:
  explicit ChartWidget(QString chartTitle, QString seriesName,
                       QWidget *parent = nullptr,
                       TrainSimulation *trainSimulation = nullptr);

private:
  QVBoxLayout *mainLayout;
  QWidget *m_chartWidget;
  QVBoxLayout *m_chartLayout;
  TrainSimulation *m_trainSimulation;
  QString m_chartTitle;

  void buildDummyLine(QString seriesName);
  void setupChart(QLineSeries *series, QString title);
  void createChartButtons(QChartView *chartView);
};

#endif // CHART_WIDGET_H
