#include "file_bridge.h"
#include <QDebug>
#include <QFile>
#include <QJsonObject>
#include <QStandardPaths>

FileBridge::FileBridge(QObject *parent) : QObject(parent) {
  // Constructor implementation
}

QJsonObject FileBridge::saveFileDialog(const QString &data,
                                       const QString &filename,
                                       const QString &filter) {
  QJsonObject result;

  // Show Qt native file save dialog
  QString suggestedPath =
      QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/" +
      filename;
  QString filepath =
      QFileDialog::getSaveFileName(nullptr, "Save File", suggestedPath, filter);

  if (filepath.isEmpty()) {
    result["success"] = false;
    result["error"] = "User cancelled file dialog";
    result["filepath"] = "";
    return result;
  }

  // Ensure file has correct extension
  if (filter.contains("*.csv") &&
      !filepath.endsWith(".csv", Qt::CaseInsensitive)) {
    filepath += ".csv";
  } else if (filter.contains("*.xlsx") &&
             !filepath.endsWith(".xlsx", Qt::CaseInsensitive)) {
    filepath += ".xlsx";
  }

  // Save file
  if (saveToFile(filepath, data)) {
    result["success"] = true;
    result["filepath"] = filepath;
    result["error"] = "";
    qDebug() << "File saved successfully:" << filepath;
  } else {
    result["success"] = false;
    result["error"] = "Failed to write file to disk";
    result["filepath"] = "";
    qDebug() << "Failed to save file:" << filepath;
  }

  return result;
}

QJsonObject FileBridge::saveBinaryFileDialog(const QVariantList &data,
                                             const QString &filename,
                                             const QString &filter) {
  QJsonObject result;

  // Show Qt native file save dialog
  QString suggestedPath =
      QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/" +
      filename;
  QString filepath =
      QFileDialog::getSaveFileName(nullptr, "Save File", suggestedPath, filter);

  if (filepath.isEmpty()) {
    result["success"] = false;
    result["error"] = "User cancelled file dialog";
    result["filepath"] = "";
    return result;
  }

  // Ensure file has correct extension
  if (filter.contains("*.xlsx") &&
      !filepath.endsWith(".xlsx", Qt::CaseInsensitive)) {
    filepath += ".xlsx";
  }

  // Convert QVariantList to QByteArray
  QByteArray byteArray;
  for (const QVariant &variant : data) {
    bool ok;
    int byteValue = variant.toInt(&ok);
    if (ok && byteValue >= 0 && byteValue <= 255) {
      byteArray.append(static_cast<char>(byteValue));
    } else {
      result["success"] = false;
      result["error"] = "Invalid byte data in array";
      result["filepath"] = "";
      return result;
    }
  }

  // Save binary file
  if (saveBinaryToFile(filepath, byteArray)) {
    result["success"] = true;
    result["filepath"] = filepath;
    result["error"] = "";
    qDebug() << "Binary file saved successfully:" << filepath;
  } else {
    result["success"] = false;
    result["error"] = "Failed to write binary file to disk";
    result["filepath"] = "";
    qDebug() << "Failed to save binary file:" << filepath;
  }

  return result;
}

bool FileBridge::saveToFile(const QString &filepath, const QString &data) {
  QFile file(filepath);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Cannot open file for writing:" << filepath
             << file.errorString();
    return false;
  }

  QTextStream out(&file);
  out.setEncoding(QStringConverter::Utf8);
  out << data;

  file.close();
  return true;
}

bool FileBridge::saveBinaryToFile(const QString &filepath,
                                  const QByteArray &data) {
  QFile file(filepath);
  if (!file.open(QIODevice::WriteOnly)) {
    qDebug() << "Cannot open binary file for writing:" << filepath
             << file.errorString();
    return false;
  }

  qint64 written = file.write(data);
  file.close();

  return written == data.size();
}

// MOC include will be handled by build system