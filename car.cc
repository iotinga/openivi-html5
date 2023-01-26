#include "car.h"
#include <QDebug>
#include <QFile>
#include <assert.h>

Car::Car(QObject *parent, DataInputMode inputMode)
    : QObject(parent), timer_(new QTimer(this)), mode(inputMode), speeds_index_(0), m_tps(0), m_rpm(0) {
  switch(mode) {
    case DATA_INPUT_FILE: {
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
    } break;
    case DATA_INPUT_CAN: {
    } break;
    default:
      // Do nothing: data will remain stopped
      break;
  }
  timer_->setInterval(1000 / 5);
  timer_->setSingleShot(false);
  timer_->start();
  connect(timer_, SIGNAL(timeout()), this, SLOT(Timer()));
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
  mode = inputMode;
}

