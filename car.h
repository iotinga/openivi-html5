#ifndef CAR_H
#define CAR_H

#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QJsonDocument>

#include <clustercandata.h>
#include <canreader.h>
#include <vector>

#define MIN_REFRESH_FREQUENCY_HZ 1
#define MAX_REFRESH_FREQUENCY_HZ 50
#define DEFAULT_REFRESH_FREQUENCY_HZ 10

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
  void setVbat(double vbatValue);
  double getVbat();

  int ParseSettingFile(const QString& path);
  void SetInputMode(DataInputMode inputMode);

  Q_PROPERTY(double rpm READ getRpm WRITE setRpm);
  Q_PROPERTY(double tps READ getTps WRITE setTps);
  Q_PROPERTY(double vbat READ getVbat WRITE setVbat);
signals:
  // void rpm(double rpm);
  void refresh_data();

 private slots:
  void Timer();

 protected:
   void clearData();
   void ParseChannelSetup(const QJsonObject& channelJson);
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
  ClusterCANData clusterCanData;
  QMutex* dataMutex;

  // Actual values to be transfered to cluster instruments
  double m_speed;
  double m_rpm;
  double m_vbat;

  // Setup info
  QJsonDocument clusterSetup;
  CANSetup canSetupInfo;
  QString canBusName;
};

#endif  // CAR_H
