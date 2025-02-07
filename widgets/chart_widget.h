#ifndef CHART_WIDGET_H
#define CHART_WIDGET_H

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
  explicit ChartWidget(QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
  QWidget *chartWidget;
  QVBoxLayout *chartLayout;
  QHBoxLayout *buttonLayout;
  QChartView *chartView;
  QChart *chart;
  void buildDummyLine();
  void setupChart(QLineSeries *series, QString title);
  void createChartButtons(QChartView *chartView);
  void setupPagination();
};

#endif // CHART_WIDGET_H
