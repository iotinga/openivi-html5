#ifndef CAR_H
#define CAR_H

#include <QObject>
#include <QTimer>
#include <QMutex>

#include <clustercandata.h>
#include <canreader.h>
#include <vector>

#define DATA_REFRESH_FREQUENCY_HZ 10

typedef enum DataInputMode_Enum
{
  DATA_INPUT_NONE,
  DATA_INPUT_FILE,
  DATA_INPUT_CAN
} DataInputMode;

class Car : public QObject {
  Q_OBJECT
 public:
  explicit Car(QObject *parent = 0);

  ~Car();

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

 protected:
   void clearData();
 private:
  // Internal timer
  QTimer *timer_;

  // Data input mode
  DataInputMode mode;

  // Speed data for file input mode
  std::vector<double> speeds_;
  size_t speeds_index_;

  // CAN data reader interface
  CanReader* canReader;
  ClusterCANData rawCanData;
  QMutex* dataMutex;

  // Actual values to be transfered to cluster instruments
  double m_tps;
  double m_rpm;
};

#endif  // CAR_H
