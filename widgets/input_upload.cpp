#include "input_upload.h"

InputUpload::InputUpload(QWidget *parent)
    : QWidget(parent), uploadButton(new QPushButton("Choose csv File", this)),
      uploadLabel(new QLabel("No file selected", this)) {
  QHBoxLayout *layout = new QHBoxLayout(this);
  layout->setContentsMargins(0, 0, 0, 0);
  uploadButton->setIcon(QIcon(":/icons/icons/upload.svg"));
  uploadButton->setIconSize(QSize(24, 24));

  layout->addWidget(uploadButton);
  layout->addWidget(uploadLabel);

  uploadButton->setStyleSheet("QPushButton{"
                              "min-width: 64px;"
                              "padding: 4px 12px; "
                              "border: 1px solid " +
                              Colors::Secondary400.name() +
                              ";"
                              "border-radius: 8px;" +
                              TextStyle::BodySmallRegular() +
                              "color: " + Colors::Secondary400.name() +
                              ";"
                              "}"
                              "QPushButton:pressed {"
                              "background-color: " +
                              Colors::Secondary50.name() +
                              ";"
                              "}");
  uploadLabel->setStyleSheet(TextStyle::SubttileSmallRegular() +
                             "color: " + Colors::Secondary700.name() + ";");
}