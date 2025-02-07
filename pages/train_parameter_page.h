#ifndef TRAINPARAMETERPAGE_H
#define TRAINPARAMETERPAGE_H

#include "../styles/colors.h"
#include "../styles/text_style.h"
#include "../widgets/button_action.h"
#include "../widgets/input_widget.h"
#include <QGroupBox>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>

class TrainParameterPage : public QWidget {
  Q_OBJECT

public:
  explicit TrainParameterPage(QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
  QStackedWidget *stackedWidget;
  QList<InputWidget *> inputWidgets;
  QList<InputWidget *> typeInputWidgets;
  QList<InputWidget *> massInputWidgets;
  QList<InputWidget *> passangerInputWidgets;
  QString groupBoxStyle;
  ButtonAction *m_prevButton;
  ButtonAction *m_nextButton;

  void setupFirstPage(QWidget *firstPageWidget);
  void setupSecondPage(QVBoxLayout *layout);
  void setupPagination();
  void showPreviousPage();
  void showNextPage();
  void updatePaginationButtons();

  QGroupBox *createTypeLayout(const QStringList &labels);
  QGroupBox *createMassLayout(const QStringList &labels);
  QGroupBox *createPassengerLayout(const QStringList &labels);
};

#endif // TRAINPARAMETERPAGE_H