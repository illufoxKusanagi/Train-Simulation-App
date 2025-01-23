#include "leftPanel.h"
#include "../styles/colors.h"
#include "../styles/textStyle.h"
#include "buttonSidebarActive.h"
#include <QDebug>
#include <QVBoxLayout>

LeftPanel::LeftPanel(QWidget *parent) : QWidget(parent) {
  QVBoxLayout *layout = new QVBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  layout->setSpacing(8);
  layout->setAlignment(Qt::AlignVCenter);
  setStyleSheet("QWidget {"
                "    background-color: " +
                Colors::Secondary500.name() +
                ";"
                "}");
  ButtonSidebarActive *constantValuesButton =
      new ButtonSidebarActive("constantActive", "Constant Values", this);
  ButtonSidebarActive *trainParameterButton =
      new ButtonSidebarActive("trainActive", "Train Parameter", this);
  ButtonSidebarActive *runningParameterButton =
      new ButtonSidebarActive("runningActive", "Running Parameter", this);
  ButtonSidebarActive *trackParameterButton =
      new ButtonSidebarActive("trackActive", "Track Parameter", this);
  ButtonSidebarActive *electricalParameterButton =
      new ButtonSidebarActive("electricalActive", "Electrical Parameter", this);
  ButtonSidebarActive *outputButton =
      new ButtonSidebarActive("outputActive", "Output", this);
  layout->addWidget(constantValuesButton);
  layout->addWidget(trainParameterButton);
  layout->addWidget(runningParameterButton);
  layout->addWidget(trackParameterButton);
  layout->addWidget(electricalParameterButton);
  layout->addWidget(outputButton);
  connect(constantValuesButton, &ButtonSidebarActive::clicked, this,
          [this]() { qDebug() << "Save all data button clicked"; });
  setLayout(layout);
}
