#include "save_button_handler.h"

SaveButtonHandler::SaveButtonHandler(TrainSimulation *trainSimulation,
                                     QString &chartTitle, QString *type)
    : m_trainSimulation(trainSimulation), m_chartTitle(chartTitle),
      m_simulationType(*type) {}

void SaveButtonHandler::onSaveAllDataClicked() {
  try {
    if (m_trainSimulation->simulationDatas.trainSpeeds.isEmpty()) {
      MessageBoxWidget messageBox(
          "No Data",
          "No simulation data to save. Please run a simulation first.",
          MessageBoxWidget::Warning);
      return;
    }

    if (m_chartTitle.contains("Dynamic") && m_simulationType != "Dynamic") {
      MessageBoxWidget messageBox("Simulation Type Mismatch",
                                  "This chart shows dynamic data but you have "
                                  "static simulation results. "
                                  "Please run a dynamic simulation first.",
                                  MessageBoxWidget::Warning);
      return;
    } else if (m_chartTitle.contains("Static") &&
               m_simulationType != "Static") {
      MessageBoxWidget messageBox("Simulation Type Mismatch",
                                  "This chart shows static data but you have "
                                  "dynamic simulation results. "
                                  "Please run a static simulation first.",
                                  MessageBoxWidget::Warning);
      return;
    }

    m_trainSimulation->m_outputHandler->printSimulationDatas();
  } catch (const std::exception &e) {
    MessageBoxWidget messageBox(
        "Error", QString("Failed to save data: %1").arg(e.what()),
        MessageBoxWidget::Critical);
  }
}

void SaveButtonHandler::onSaveCurrentDataClicked() {
  try {
    if (m_trainSimulation->simulationDatas.trainSpeeds.isEmpty()) {
      MessageBoxWidget messageBox(
          "No Data",
          "No simulation data to save. Please run a simulation first.",
          MessageBoxWidget::Warning);
      return;
    }
    if (m_chartTitle.contains("Dynamic") && m_simulationType != "Dynamic") {
      MessageBoxWidget messageBox("Simulation Type Mismatch",
                                  "This chart shows dynamic data but you have "
                                  "static simulation results. "
                                  "Please run a dynamic simulation first.",
                                  MessageBoxWidget::Warning);
      return;
    } else if (m_chartTitle.contains("Static") &&
               m_simulationType != "Static") {
      MessageBoxWidget messageBox("Simulation Type Mismatch",
                                  "This chart shows static data but you have "
                                  "dynamic simulation results. "
                                  "Please run a static simulation first.",
                                  MessageBoxWidget::Warning);
      return;
    }
    bool saveSuccessful = false;
    if (m_chartTitle == "Dynamic Power" || m_chartTitle == "Static Power" ||
        m_chartTitle == "Dynamic Current" || m_chartTitle == "Static Current")
      saveSuccessful = m_trainSimulation->m_outputHandler->saveTrainPowerData();
    else if (m_chartTitle == "Traction Effort" ||
             m_chartTitle == "Static Traction Effort")
      saveSuccessful =
          m_trainSimulation->m_outputHandler->saveTractionEffortData();
    else if (m_chartTitle == "Dynamic Speed" || m_chartTitle == "Static Speed")
      saveSuccessful = m_trainSimulation->m_outputHandler->saveTrainSpeedData();
    else if (m_chartTitle == "Distance")
      saveSuccessful = m_trainSimulation->m_outputHandler->saveTrainTrackData();
    else if (m_chartTitle == "Dynamic Energy" ||
             m_chartTitle == "Static Energy")
      saveSuccessful =
          m_trainSimulation->m_outputHandler->saveEnergyConsumptionData();
    if (saveSuccessful) {
      MessageBoxWidget messageBox("Success", "Data saved successfully!",
                                  MessageBoxWidget::Information);
    }

  } catch (const std::exception &e) {
    MessageBoxWidget messageBox(
        "Error", QString("Failed to save data: %1").arg(e.what()),
        MessageBoxWidget::Critical);
  }
}

void SaveButtonHandler::onSaveButtonClicked(QChartView *chartView) {
  QString filePath = QFileDialog::getSaveFileName(
      nullptr, tr("Save Chart"), QDir::homePath(),
      tr("Images (*.png *.jpg *.bmp);;All Files (*)"));

  if (!filePath.isEmpty()) {
    QPixmap pixmap = chartView->grab();
    if (pixmap.save(filePath)) {
      MessageBoxWidget messageBox(
          "Save Successful", QString("Chart image saved at %1").arg(filePath),
          MessageBoxWidget::Information);
    } else {
      MessageBoxWidget messageBox(
          "Save Failed",
          QString("Could not save the chart to %1").arg(filePath) +
              "the file path and try again.",
          MessageBoxWidget::Warning);
    }
  }
}
