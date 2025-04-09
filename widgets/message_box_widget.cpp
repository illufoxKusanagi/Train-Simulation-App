#include "message_box_widget.h"

MessageBoxWidget::MessageBoxWidget() {
  setWindowTitle("Information");
  setText("Simulation completed.");
  setIcon(QMessageBox::Information);
  setStandardButtons(QMessageBox::Ok);
  setStyleSheet("QMessageBox QLabel {"
                "    color:" +
                Colors::StandardBlack.name() + ";" +
                TextStyle::BodyMediumRegular() +
                "}"
                "QPushButton {"
                "    background-color: " +
                Colors::Primary500.name() +
                ";"
                "    color: white;"
                "    border: none;"
                "    padding: 6px 12px;"
                "    border-radius: 4px;"
                "}"
                "QPushButton:hover {"
                "    background-color: " +
                Colors::Primary600.name() +
                ";"
                "}");
}
