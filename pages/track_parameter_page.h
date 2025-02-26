#ifndef TRACK_PARAMETER_PAGE_H
#define TRACK_PARAMETER_PAGE_H

#include "../widgets/input_widget.h"
#include <QGridLayout>
#include <QWidget>

class TrackParameterPage : public QWidget {
  Q_OBJECT
public:
  explicit TrackParameterPage(QWidget *parent = nullptr);

private:
  QVBoxLayout *mainLayout;
  QWidget *m_formContainer;
  QGridLayout *m_formLayout;
  QMap<QString, InputWidget *> m_inputWidgets;

  void createInputs();
};

#endif // TRACK_PARAMETER_PAGE_H
