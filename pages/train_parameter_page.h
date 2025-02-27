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
  double getParameterValue(const QString &paramName);

private:
  QVBoxLayout *mainLayout;
  QStackedWidget *stackedWidget;
  QMap<QString, InputWidget *> m_inputWidgets;
  QMap<QString, InputWidget *> m_typeInputWidgets;
  QMap<QString, InputWidget *> m_massInputWidgets;
  QMap<QString, InputWidget *> m_passengerInputWidgets;
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

  QGroupBox *createTypeLayout(const QStringList &labels, QList<double> values);
  QGroupBox *createMassLayout(const QStringList &labels, QList<double> values);
  QGroupBox *createPassengerLayout(const QStringList &labels,
                                   QList<double> values);
};

#endif // TRAINPARAMETERPAGE_H