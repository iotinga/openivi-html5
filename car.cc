#include "car.h"
#include <QDebug>
#include <QFile>
#include <assert.h>

Car::Car(QObject *parent)
    : QObject(parent), timer_(new QTimer(this)), mode(DATA_INPUT_NONE), speeds_index_(0), m_tps(0), m_rpm(0) {
  dataMutex = new QMutex;
  canReader = NULL;
  clearData();

  timer_->setInterval(1000 / DATA_REFRESH_FREQUENCY_HZ);
  timer_->setSingleShot(false);
  timer_->start();
  connect(timer_, SIGNAL(timeout()), this, SLOT(Timer()));
}

Car::~Car()
{
  delete dataMutex;
  dataMutex = NULL;
  if (canReader != NULL) {
    canReader->requestInterruption();
    canReader->wait();
    delete canReader;
    canReader = NULL;
  }
}


void Car::Timer() {
  switch (mode) {
    case DATA_INPUT_FILE: {
      speeds_index_++;
      m_rpm = speeds_[speeds_index_] * 400;
      m_tps = speeds_index_;
      refresh_data();
      if (speeds_.size() <= speeds_index_) {
        speeds_index_ = 0;
      }
    } break;
    case DATA_INPUT_CAN: {
      if (canReader != NULL) {
        dataMutex->lock();
        m_rpm = (double) rawCanData.rpm_raw;
        m_tps = (double) rawCanData.tps_raw;
        dataMutex->unlock();
        refresh_data();
      }
    } break;
    default:
      break;
  }
}

void Car::setTps(double tpsValue) {
  m_tps = tpsValue;
}

double Car::getTps() {
  return m_tps;
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
        canReader = new CanReader(&rawCanData, dataMutex);
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
  m_tps = 0.0;
  m_rpm = 0.0;
  dataMutex->lock();
  rawCanData.rpm_raw = 0;
  rawCanData.tps_raw = 0;
  dataMutex->unlock();
}
