#ifndef LEFT_PANEL_H
#define LEFT_PANEL_H

#include "../models/train_simulation.h"
#include "../widgets/button_action.h"
#include "button_toggle.h"
#include "left_panel_category.h"
#include <QFuture>
#include <QFutureWatcher>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QtConcurrent/QtConcurrent>

class LeftPanel : public QWidget {
  Q_OBJECT

public:
  explicit LeftPanel(QWidget *parent = nullptr,
                     TrainSimulation &trainSimulation = defaultTrainSimulation);
  static TrainSimulation defaultTrainSimulation;
public slots:
  void emitNavigateSignal(int pageIndex);

signals:
  void navigateToPage(int pageIndex);

private:
  bool m_isCollapsed = false;
  int m_currentIndex = -1;
  ButtonToggle *m_buttonToggle;
  QVBoxLayout *m_buttonLayout;
  LeftPanelInputs *m_inputPanel;
  LeftPanelInputs *m_outputPanel;
  TrainSimulation *m_trainSimulation;

  int getCurrentIndex() const { return m_currentIndex; }
  void setCurrentIndex(int index) { m_currentIndex = index; }
  void createRunButton();
  void setupInputPageButtons();
  void setupOutputPageButtons();
  void updateButtonState(QFuture<void> future, ButtonAction *runButton,
                         ButtonAction *staticRunButton);
};

#endif // LEFT_PANEL_H
