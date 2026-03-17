#include "user_manager.h"

#include <QDebug>
#include <QCoreApplication>
#include <QMessageAuthenticationCode>
#include <QCryptographicHash>

UserManager::UserManager(QObject *parent) : QObject(parent) {
  QString authPath = QCoreApplication::applicationDirPath() + "/.auth";
  QFile file(authPath);
  
  // If file doesn't exist, create default
  if (!file.exists()) {
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&file);
      out << "ADMIN_USERNAME=admin\n";
      // Hash of "admin123"
      QByteArray hash = QCryptographicHash::hash(QString("admin123").toUtf8(), QCryptographicHash::Sha256);
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
  } else {
    m_loaded = false;
    qWarning() << "Failed to open .auth file at" << authPath;
  }
}
