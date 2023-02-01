#include "car.h"
#include <QDebug>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <assert.h>

Car::Car(QObject *parent)
    : QObject(parent), timer_(new QTimer(this)), mode(DATA_INPUT_NONE), speeds_index_(0), m_speed(0), m_rpm(0) {
  dataMutex = new QMutex;
  canReader = NULL;
  clearData();
  // NULL pointers to channels, will be initialized during setup parsing
  clusterCanData.rpm_ch = NULL;
  clusterCanData.speed_ch = NULL;

  /* Default port setup. */
  canSetupInfo.baudrate = 500000;
  canSetupInfo.bus_mode = CAN_MODE_NORMAL;
  canSetupInfo.enableCAN_FD = false;
  canSetupInfo.filterCount = 0;
  canSetupInfo.filterIDs = NULL;
  canSetupInfo.frame_format = CAN_FRAME_11;
  canSetupInfo.timeout_ms = 1000;
  canSetupInfo.endianess = CAN_ENDIAN_BIG;

  timer_->setInterval(1000 / DEFAULT_REFRESH_FREQUENCY_HZ);
  timer_->setSingleShot(false);
  timer_->start();
  connect(timer_, SIGNAL(timeout()), this, SLOT(Timer()));
}

Car::~Car()
{
  // First stop thread, then can clear data
  if (canReader != NULL) {
    canReader->requestInterruption();
    canReader->wait();
    delete canReader;
    canReader = NULL;
  }
  delete dataMutex;
  dataMutex = NULL;
}


void Car::Timer() {
  switch (mode) {
    case DATA_INPUT_FILE: {
      speeds_index_++;
      m_rpm = speeds_[speeds_index_] * 400;
      m_speed = speeds_index_;
      refresh_data();
      if (speeds_.size() <= speeds_index_) {
        speeds_index_ = 0;
      }
    } break;
    case DATA_INPUT_CAN: {
      if (canReader != NULL) {
        dataMutex->lock();
        if (clusterCanData.rpm_ch != NULL) {
          m_rpm = clusterCanData.rpm_ch->GetCalibratedValue(clusterCanData.rpm_raw);
        }
        if (clusterCanData.speed_ch != NULL) {
          m_speed = clusterCanData.speed_ch->GetCalibratedValue(clusterCanData.speed_raw);
        }
        dataMutex->unlock();
        refresh_data();
      }
    } break;
    default:
      break;
  }
}

void Car::setTps(double tpsValue) {
  m_speed = tpsValue;
}

double Car::getTps() {
  return m_speed;
}

void Car::setRpm(double rpmValue) {
  m_rpm = rpmValue;
}

double Car::getRpm() {
  return m_rpm;
}

void Car::SetInputMode(DataInputMode inputMode)
{
  clearData();
  mode = inputMode;

  switch(mode) {
    case DATA_INPUT_FILE: {
      if (speeds_.size() == 0) {
        QFile speeds(":/help/speeds.txt");
        if (speeds.open(QIODevice::ReadOnly | QIODevice::Text)) {
          QTextStream in(&speeds);
          for (QString line = in.readLine(); !line.isNull(); line = in.readLine()) {
            double speed = line.toDouble();
            speeds_.push_back(speed);
          }
        } else {
          qDebug() << "failed to open speeds resource";
          speeds_.push_back(50);
        }
        assert(speeds_.size() > 0);
      }
    } break;
    case DATA_INPUT_CAN: {
      if (canReader == NULL) {
        std::string busName = canBusName.toUtf8().toStdString();
        canReader = new CanReader(busName.c_str(), &canSetupInfo, &clusterCanData, dataMutex);
        canReader->start();
      }
    } break;
    default:
      // Do nothing: data will remain stopped
      break;
  }
}

void Car::clearData()
{
  m_speed = 0.0;
  m_rpm = 0.0;
  dataMutex->lock();
  clusterCanData.rpm_raw = 0;
  clusterCanData.speed_raw = 0;
  dataMutex->unlock();
}

int Car::ParseSettingFile(const QString& path)
{
  int ch = 0;
  if (path.size() > 0) {
    QFile loadFile(path);

    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open setting file.");
        return -1;
    }

    QByteArray saveData = loadFile.readAll();

    clusterSetup = QJsonDocument::fromJson(saveData);

    if (clusterSetup.isObject()) {
      clusterCanData.can_filter_set.clear();

      QJsonObject rootObject = clusterSetup.object();
      // Parse "graphics" section
      if (rootObject.contains("graphics") && rootObject["graphics"].isObject()) {
        QJsonObject graphicsObject = rootObject["graphics"].toObject();
        if (graphicsObject.contains("refresh_frequency")) {
          int frequency = graphicsObject["refresh_frequency"].toInt();
          if ((frequency >= MIN_REFRESH_FREQUENCY_HZ) && (frequency <= MAX_REFRESH_FREQUENCY_HZ)) {
            timer_->setInterval(1000/frequency);
          }
        }
      }
      // Parse "can_bus" section, which is CAN port setup
      if (rootObject.contains("can_bus") && rootObject["can_bus"].isObject()) {
        QJsonObject canObject = rootObject["can_bus"].toObject();
        if (canObject.contains("bus_name")) {
          canBusName = canObject["bus_name"].toString();
        }
        if (canObject.contains("baudrate")) {
          canSetupInfo.baudrate = (uint32_t) canObject["baudrate"].toInt();
        }
        if (canObject.contains("timeout")) {
          canSetupInfo.timeout_ms = (uint32_t) canObject["timeout"].toInt();
        }
        if (canObject.contains("fd")) {
          canSetupInfo.enableCAN_FD = (uint32_t) canObject["fd"].toBool();
        }
        if (canObject.contains("id_format")) {
          if (canObject["id_format"].toString().compare("standard")) {
            canSetupInfo.frame_format = CAN_FRAME_11;
          } else {
            canSetupInfo.frame_format = CAN_FRAME_29;
          }
        }
        if (canObject.contains("endianess")) {
          if (canObject["endianess"].toString().compare("little") == 0) {
            canSetupInfo.endianess = CAN_ENDIAN_LITTLE;
          } else {
            canSetupInfo.endianess = CAN_ENDIAN_BIG;
          }
        }

      }
      if (rootObject.contains("channels") && rootObject["channels"].isArray()) {
        QJsonArray channelsArray = rootObject["channels"].toArray();
        for (ch = 0; ch < channelsArray.size(); ch++) {
          QJsonObject channelObject = channelsArray[ch].toObject();
          ParseChannelSetup(channelObject);
        }
      }
    }
  }
  return 0;
}

void Car::ParseChannelSetup(const QJsonObject& channelJson)
{
  std::string channelName;
  std::string channelCanIdStr;
  CANChannel* channelObj = NULL;
  canid_t channelID = 0x00000000;
  uint32_t channelStart = 0;
  uint32_t channelBitSize = 0;
  double channelGain = 0.0, channelOffset = 0.0;

  if (channelJson.contains("name")) {
    channelName.assign(channelJson["name"].toString().toUtf8().toStdString());

    if (channelName.size() > 0) {
      if (channelName.compare("speed") == 0) {
        if (clusterCanData.speed_ch == NULL) {
          clusterCanData.speed_ch = new CANChannel("speed");
        }
        channelObj = clusterCanData.speed_ch;
      } else if (channelName.compare("rpm") == 0) {
        if (clusterCanData.rpm_ch == NULL) {
          clusterCanData.rpm_ch = new CANChannel("rpm");
        }
        channelObj = clusterCanData.rpm_ch;
      }
    }
  }
  if (channelObj) {
    // Parse CAN parameters
    if (channelJson.contains("can_id")) {
      channelCanIdStr.assign(channelJson["can_id"].toString().toUtf8().toStdString());
      channelID = strtol(channelCanIdStr.c_str(), NULL, 16);
    }
    if (channelJson.contains("startbit")) {
      channelStart = channelJson["startbit"].toInt();
    }
    if (channelJson.contains("bitlength")) {
      channelBitSize = channelJson["bitlength"].toInt();
    }
    channelObj->SetCANParameters(channelID, channelStart, channelBitSize, canSetupInfo.enableCAN_FD ? CANFD_MAX_DLEN : CAN_MAX_DLEN);
    if (channelID != 0x00000000) {
      clusterCanData.can_filter_set.insert(channelID);
    }

    // Parse calibration parameters
    if (channelJson.contains("gain")) {
      channelGain = channelJson["gain"].toDouble();
    }
    if (channelJson.contains("offset")) {
      channelOffset = channelJson["offset"].toDouble();
    }
    channelObj->SetCalibrationParameters(channelGain, channelOffset);
  }
}
