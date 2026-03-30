#include "file_bridge.h"
#include <QCoreApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QEventLoop>
#include <QFile>
#include <QFileDialog>
#include <QJsonObject>
#include <QStandardPaths>
#include <QTimer>
#include <QUrl>

FileBridge::FileBridge(QObject *parent) : QObject(parent) {}

QJsonObject FileBridge::saveFileDialog(const QString &data,
                                       const QString &filename,
                                       const QString &filter) {
  QJsonObject result;

  QString filepath;
  QEventLoop loop;
  QTimer::singleShot(0, [&]() {
    QString suggestedPath =
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/" +
        filename;
    filepath = QFileDialog::getSaveFileName(nullptr, "Save File", suggestedPath,
                                            filter);
    loop.quit();
  });
  loop.exec();

  QCoreApplication::processEvents();

  if (filepath.isEmpty()) {
    result["success"] = false;
    result["error"] = "User cancelled file dialog";
    result["filepath"] = "";
    return result;
  }

  if (filter.contains("*.csv") &&
      !filepath.endsWith(".csv", Qt::CaseInsensitive)) {
    filepath += ".csv";
  } else if (filter.contains("*.xlsx") &&
             !filepath.endsWith(".xlsx", Qt::CaseInsensitive)) {
    filepath += ".xlsx";
  }

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

  QString filepath;
  QEventLoop loop;
  QTimer::singleShot(0, [&]() {
    QString suggestedPath =
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/" +
        filename;
    filepath = QFileDialog::getSaveFileName(nullptr, "Save File", suggestedPath,
                                            filter);
    loop.quit();
  });
  loop.exec();

  QCoreApplication::processEvents();

  if (filepath.isEmpty()) {
    result["success"] = false;
    result["error"] = "User cancelled file dialog";
    result["filepath"] = "";
    return result;
  }

  if (filter.contains("*.xlsx") &&
      !filepath.endsWith(".xlsx", Qt::CaseInsensitive)) {
    filepath += ".xlsx";
  }

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

void FileBridge::openUrl(const QString &url) {
  QDesktopServices::openUrl(QUrl(url));
}

QJsonObject FileBridge::openFileDialog(const QString &title,
                                       const QString &filter) {
  QJsonObject result;

  QString filepath;
  QEventLoop loop;
  QTimer::singleShot(0, [&]() {
    filepath = QFileDialog::getOpenFileName(
        nullptr, title,
        QStandardPaths::writableLocation(QStandardPaths::HomeLocation), filter);
    loop.quit();
  });
  loop.exec();

  if (filepath.isEmpty()) {
    result["success"] = false;
    result["error"] = "User cancelled file dialog";
    result["content"] = "";
    return result;
  }

  QFile file(filepath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    result["success"] = false;
    result["error"] = "Could not open file for reading";
    result["content"] = "";
    return result;
  }

  QTextStream in(&file);
  in.setEncoding(QStringConverter::Utf8);
  QString content = in.readAll();
  file.close();

  QFileInfo info(filepath);
  result["success"] = true;
  result["content"] = content;
  result["filename"] = info.fileName();
  result["error"] = "";
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
