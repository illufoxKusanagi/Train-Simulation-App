#include "user_manager.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>

UserManager::UserManager(QObject *parent) : QObject(parent) {
  QString dataDir =
      QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
  QDir().mkpath(dataDir);
  QString authPath = dataDir + "/.auth";
  QFile file(authPath);

  if (!file.exists()) {
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&file);
      out << "ADMIN_USERNAME=admin\n";
      out << "ADMIN_PASSWORD_HASH="
             "100000:696e6b61737461746963:"
             "ce19571f3cc61f0dc6f4960ef15ac6413919f6d0b9b9073f2a31c8aa2a9141a4"
             "\n";
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
      if (line.startsWith("ADMIN_USERNAME=")) {
        m_username = line.section('=', 1);
      } else if (line.startsWith("ADMIN_PASSWORD_HASH=")) {
        QString fullHash = line.section('=', 1);
        QStringList parts = fullHash.split(':');
        if (parts.size() == 3) {
          m_passwordIterations = parts[0].toInt();
          m_passwordSalt = parts[1];
          m_passwordHash = parts[2];
        } else if (parts.size() == 1) {
          m_passwordIterations = 1;
          m_passwordSalt = "";
          m_passwordHash = parts[0];
        }
      }
    }
    file.close();
    m_loaded = !(m_username.isEmpty() || m_passwordHash.isEmpty());
    qInfo() << "Auth credentials loaded from" << authPath;
  } else {
    m_loaded = false;
    qWarning() << "Failed to open .auth file at" << authPath;
  }
}
