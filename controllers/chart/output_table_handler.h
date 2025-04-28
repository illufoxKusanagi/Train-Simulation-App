#ifndef OUTPUT_TABLE_HANDLER_H
#define OUTPUT_TABLE_HANDLER_H

#include "controllers/train_simulation/train_simulation.h"
#include "core/appcontext.h"
#include "styles/colors.h"
#include "styles/text_style.h"
#include <QHeaderView>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QWidget>

class OutputTableHandler : public QObject {
  Q_OBJECT
public:
  explicit OutputTableHandler(TrainSimulation *trainSimulation,
                              QVBoxLayout *layout, QString chartTitle,
                              AppContext *context = nullptr);
  void setupTable();
  void updateTable();
  QTableWidget *m_table;

private:
  QVBoxLayout *mainLayout;
  TrainSimulation *m_trainSimulation;
  QString m_chartTitle;
  AppContext *context;
};

#endif // OUTPUT_TABLE_HANDLER_H
