#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QStandardPaths>
#include <QString>
#include <QTextStream>

class UserManager : public QObject {
  Q_OBJECT
public:
  UserManager(QObject *parent = nullptr);
  QString username() const { return m_username; }
  QString passwordHash() const { return m_passwordHash; }
  QString passwordSalt() const { return m_passwordSalt; }
  int passwordIterations() const { return m_passwordIterations; }
  bool isLoaded() const { return m_loaded; }

private:
  QString m_username;
  QString m_passwordHash;
  QString m_passwordSalt;
  int m_passwordIterations = 100000;
  bool m_loaded = false;
};

#endif // USER_MANAGER_H
