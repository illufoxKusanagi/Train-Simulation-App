#include "track_parameter_page.h"

TrackParameterPage::TrackParameterPage(QWidget *parent)
    : QWidget(parent), mainLayout(new QVBoxLayout(this)),
      formContainer(new QWidget(this)),
      formLayout(new QGridLayout(formContainer)) {
  mainLayout->setContentsMargins(16, 16, 16, 16);
  mainLayout->setAlignment(Qt::AlignCenter);
  formLayout->setHorizontalSpacing(80);
  formLayout->setVerticalSpacing(32);
  createInputs();
  mainLayout->addWidget(formContainer);
  setLayout(mainLayout);
}

void TrackParameterPage::createInputs() {
  QStringList labels = {"Number of Station", "Radius per Section",
                        "Station Distance",  "Slope per Section",
                        "Section Distance",  "Speed Limit"};
  QString unit = "m/s^2";
  for (int i = 0; i < labels.size(); i++) {
    InputWidget *inputWidget = new InputWidget(
        InputType(i > 0 ? "upload" : "field", labels[i], i > 0 ? "" : unit),
        this);
    formLayout->addWidget(inputWidget, i / 2, i % 2);
  }
}