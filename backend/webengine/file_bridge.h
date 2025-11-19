#ifndef FILE_BRIDGE_H
#define FILE_BRIDGE_H

#include <QByteArray>
#include <QDir>
#include <QFileDialog>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QVariantList>

class FileBridge : public QObject {
  Q_OBJECT

public:
  explicit FileBridge(QObject *parent = nullptr);

public slots:
  /**
   * @brief Show native Qt file save dialog and save the data
   * @param data The file content as string (for CSV) or base64 for binary files
   * @param filename Suggested filename
   * @param filter File filter (e.g., "CSV Files (*.csv)")
   * @return JSON object with success status and filepath
   */
  QJsonObject saveFileDialog(const QString &data, const QString &filename,
                             const QString &filter);

  /**
   * @brief Save binary data (Excel files) using native dialog
   * @param data Binary data as QVariantList (JavaScript array converted to Qt)
   * @param filename Suggested filename
   * @param filter File filter (e.g., "Excel Files (*.xlsx)")
   * @return JSON object with success status and filepath
   */
  QJsonObject saveBinaryFileDialog(const QVariantList &data,
                                   const QString &filename,
                                   const QString &filter);

private:
  bool saveToFile(const QString &filepath, const QString &data);
  bool saveBinaryToFile(const QString &filepath, const QByteArray &data);
};

#endif // FILE_BRIDGE_H