#ifndef CHART_WIDGET_H
#define CHART_WIDGET_H

#include "controllers/chart/save_button_handler.h"
#include "controllers/train_simulation//tractive_effort_handler.h"
#include "controllers/train_simulation//train_simulation.h"
#include "styles/colors.h"
#include "styles/text_style.h"
#include "widgets/button_action.h"
#include "widgets/button_pagination.h"
#include "widgets/message_box_widget.h"
#include <QAbstractSeries>
#include <QFileDialog>
#include <QFont>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QTableView>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QValueAxis>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

class ChartWidget : public QWidget {
  Q_OBJECT
public:
  explicit ChartWidget(QWidget *parent = nullptr, QString chartTitle = "",
                       QString seriesName = "",
                       TrainSimulation *trainSimulation = nullptr);
  void addSeries(const QString &name, const QColor &color);
  void updateChart();
  void updateStaticChart();

private slots:
  void onSimulationCompleted();
  void onStaticSimulationCompleted();
  void onSaveAllDataClicked();
  void onSaveCurrentDataClicked();
  void onSaveButtonClicked(QChartView *chartView);

private:
  QVBoxLayout *mainLayout;
  QWidget *m_chartWidget;
  QVBoxLayout *m_chartLayout;
  TrainSimulation *m_trainSimulation;
  QString m_chartTitle;
  QChart *m_chart;
  QList<QString> m_seriesToDisplay;
  QMap<QString, QColor> m_seriesColors;
  QTableWidget *m_table;
  // enum SimulationType { None, Dynamic, Static };
  SaveButtonHandler::SimulationType m_simulationType;
  // SimulationType m_simulationType = None;
  TractiveEffortHandler *m_tractiveEffortHandler;
  SaveButtonHandler *m_saveButtonHandler;

  void buildDummyLine(QString seriesName);
  void setupChart(QLineSeries *series, QString title);
  void setupTable();
  void updateTable();
  void createChartButtons(QChartView *chartView);

  void setupStaticSpeedChart();
  void setupStaticTractionChart();
  void setupStaticTrackChart();
  void setupStaticPowerChart();
  void setupStaticCurrentChart();
  void setupStaticEnergyChart();

  void setupDynamicSpeedChart();
  void setupDynamicTractionChart();
  void setupDynamicTrackChart();
  void setupDynamicPowerChart();
  void setupDynamicCurrentChart();
  void setupDynamicEnergyChart();
  void setupDistanceChart();

  void setupStaticAxis();
  void setupDynamicAxis();
};

#endif // CHART_WIDGET_H
