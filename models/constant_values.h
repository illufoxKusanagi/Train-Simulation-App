#ifndef CONSTANT_VALUES_H
#define CONSTANT_VALUES_H

#include <QObject>

class ConstantValues : public QObject {
  Q_OBJECT
public:
  explicit ConstantValues(QObject *parent = nullptr);
  double getGravitation() const;
  double getSpeedConverter() const;
  double getForceConverter() const;

private:
  double m_gravitation;
  double m_speedConverter;
  double m_forceConverter;
};

#endif // CONSTANT_VALUES_H
