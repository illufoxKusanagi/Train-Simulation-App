#include "user_manager.h"

#include <QDebug>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QStandardPaths>

UserManager::UserManager(QObject *parent) : QObject(parent) {
  // Use a user-writable app-data directory so this works regardless of
  // where the binary is installed (avoids permission errors in /opt, C:\Program Files, etc.)
  QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir().mkpath(dataDir);
  QString authPath = dataDir + "/.auth";
  QFile file(authPath);

  // If file doesn't exist, create default credentials
  if (!file.exists()) {
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&file);
      out << "ADMIN_USERNAME=admin\n";
      QByteArray hash = QCryptographicHash::hash(
          QString("admin123").toUtf8(), QCryptographicHash::Sha256);
      out << "ADMIN_PASSWORD_HASH=" << hash.toHex() << "\n";
      file.close();
      qInfo() << "Created default .auth file at" << authPath;
    } else {
      qWarning() << "Failed to create default .auth file at" << authPath;
    }
  }

  if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    while (!in.atEnd()) {
      QString line = in.readLine().trimmed();
      if (line.startsWith("ADMIN_USERNAME="))
        m_username = line.section('=', 1);
      else if (line.startsWith("ADMIN_PASSWORD_HASH="))
        m_passwordHash = line.section('=', 1);
    }
    file.close();
    m_loaded = !(m_username.isEmpty() || m_passwordHash.isEmpty());
    qInfo() << "Auth credentials loaded from" << authPath;
  } else {
    m_loaded = false;
    qWarning() << "Failed to open .auth file at" << authPath;
  }
}
