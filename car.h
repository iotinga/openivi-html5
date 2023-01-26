#ifndef CAR_H
#define CAR_H

#include <QObject>
#include <QTimer>

#include <clustercandata.h>
#include <vector>

typedef enum DataInputMode_Enum
{
  DATA_INPUT_NONE,
  DATA_INPUT_FILE,
  DATA_INPUT_CAN
} DataInputMode;

class Car : public QObject {
  Q_OBJECT
 public:
  explicit Car(QObject *parent = 0, DataInputMode inputMode = DATA_INPUT_NONE);

  void setTps(double tpsValue);
  double getTps();
  void setRpm(double rpmValue);
  double getRpm();

  void SetInputMode(DataInputMode inputMode);

  Q_PROPERTY(double rpm READ getRpm WRITE setRpm);
  Q_PROPERTY(double tps READ getTps WRITE setTps);
signals:
  // void rpm(double rpm);
  void refresh_data();

 private slots:
  void Timer();

 private:
  QTimer *timer_;
  DataInputMode mode;
  std::vector<double> speeds_;
  size_t speeds_index_;
  double m_tps;
  double m_rpm;
};

#endif  // CAR_H
