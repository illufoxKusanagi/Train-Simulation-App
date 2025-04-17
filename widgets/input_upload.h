#ifndef INPUTUPLOAD_H
#define INPUTUPLOAD_H

#include "message_box_widget.h"
#include "styles/colors.h"
#include "styles/text_style.h"
#include <QBoxLayout>
#include <QFileDialog>
#include <QLabel>
#include <QPushButton>


class InputUpload : public QWidget {
  Q_OBJECT

public:
  explicit InputUpload(QWidget *parent = nullptr);
  // Set minimum number of required columns
  void setRequiredColumnCount(int count);
  // Set which specific columns to extract (by index)
  void setTargetColumns(const QList<int> &columnIndices);
  // Get data from a specific column index
  QList<double> getColumnData(int columnIndex) const;
  // Check if file has minimum columns
  bool hasRequiredColumns() const;
  bool validateColumnCount(const QStringList &values);
  QString getFilePath() const;
  bool isFileLoaded() const;
  // Get all parsed data as a list of row data
  QList<QList<double>> getAllData() const;

signals:
  void fileLoaded(const QString &filePath);
  void fileLoadError(const QString &errorMessage);
  void uploadButtonClicked();

private slots:
  void onUploadButtonClicked();

private:
  QPushButton *m_uploadButton;
  QLabel *m_uploadLabel;
  QString m_filePath;
  int m_requiredColumnCount;
  QList<int> m_targetColumns;
  QList<QList<double>> m_data;

  bool readCsvFile(const QString &filePath);
  bool validateColumns(const QStringList &values);
};

#endif // INPUTUPLOAD_H
