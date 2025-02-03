#ifndef OUTPUT_PAGE_H
#define OUTPUT_PAGE_H

#include "../widgets/button_action.h"
#include <QFileDialog>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QLineSeries>

class OutputPage : public QWidget {
  Q_OBJECT

public:
  explicit OutputPage(QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
  QStackedWidget *stackedWidget;
  QWidget *pageContainer;
  QVBoxLayout *pageLayout;
  QVBoxLayout *chartLayout;
  QHBoxLayout *buttonLayout;
  QChartView *chartView;
  QChart *chart;
  void setupFirstPage();
  void setupSecondPage();
  void setupThirdPage();
  void setupChart(QLineSeries *series);
  void createChartButtons(QChartView *chartView);
  void createActionButtons();
  void setupPagination();
};

#endif // OUTPUT_PAGE_H
