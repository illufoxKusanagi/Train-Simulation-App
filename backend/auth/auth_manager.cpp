#include "auth_manager.h"
#include <QPasswordDigestor>
#include <qdebug.h>

AuthManager::AuthManager(UserManager *userManager, QObject *parent)
    : QObject(parent), m_userManager(userManager) {}

bool AuthManager::login(const QString &username, const QString &password) {
  if (!m_userManager || !m_userManager->isLoaded()) {
    return false;
  }

  QString inputHash = hashPassword(password, m_userManager->passwordSalt(),
                                   m_userManager->passwordIterations());
  if (username == m_userManager->username() &&
      inputHash == m_userManager->passwordHash()) {
    m_authenticated = true;
    return true;
  }
  m_authenticated = false;
  return false;
}

bool AuthManager::isAuthenticated() const { return m_authenticated; }

QString AuthManager::hashPassword(const QString &password,
                                  const QString &saltHex, int iterations) {
  if (saltHex.isEmpty() || iterations <= 1) {
    QByteArray hash =
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
    return QString(hash.toHex());
  }

  QByteArray salt = QByteArray::fromHex(saltHex.toUtf8());
  QByteArray hash = QPasswordDigestor::deriveKeyPbkdf2(
      QCryptographicHash::Sha256, password.toUtf8(), salt, iterations, 32);

  return QString(hash.toHex());
}

bool AuthManager::processLogin() {
  bool isLoggedIn = false;
  return isLoggedIn;
}
