#ifndef AUTH_MANAGER_H
#define AUTH_MANAGER_H

#include "user_manager.h"
#include <QCryptographicHash>
#include <QFile>
#include <QString>
#include <QTextStream>

class AuthManager : public QObject {
  Q_OBJECT
public:
  explicit AuthManager(UserManager *userManager, QObject *parent = nullptr);

  bool login(const QString &username, const QString &password);
  bool isAuthenticated() const;
  bool processLogin();

private:
  UserManager *m_userManager;
  bool m_authenticated = false;

  QString hashPassword(const QString &password, const QString &saltHex, int iterations);
};

#endif // AUTH_MANAGER_H
