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
  explicit ChartWidget(QString chartTitle, QString seriesName,
                       QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
  QWidget *m_chartWidget;
  QVBoxLayout *m_chartLayout;
  void buildDummyLine(QString chartTitle, QString seriesName);
  void setupChart(QLineSeries *series, QString title);
  void createChartButtons(QChartView *chartView);
};

#endif // CHART_WIDGET_H
