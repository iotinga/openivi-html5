#ifndef CAR_H
#define CAR_H

#include <QObject>
#include <QTimer>

#include <clustercandata.h>
#include <vector>

class Car : public QObject {
  Q_OBJECT
 public:
  explicit Car(QObject *parent = 0);

  void setTps(double tpsValue);
  double getTps();
  void setRpm(double rpmValue);
  double getRpm();

  Q_PROPERTY(double rpm READ getRpm WRITE setRpm);
  Q_PROPERTY(double tps READ getTps WRITE setTps);
signals:
  // void rpm(double rpm);
  void refresh_data();

 private slots:
  void Timer();

 private:
  QTimer *timer_;
  std::vector<double> speeds_;
  size_t speeds_index_;
  double m_tps;
  double m_rpm;
};

#endif  // CAR_H
