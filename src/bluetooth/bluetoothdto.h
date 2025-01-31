#ifndef BLUETOOTHDEVICE_H
#define BLUETOOTHDEVICE_H

#include <QBluetoothAddress>
#include <QBluetoothDeviceInfo>
#include <QObject>
#include <QString>

class BluetoothDevice : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString address MEMBER m_address)
    Q_PROPERTY(QString name MEMBER m_name)

  public:
    explicit BluetoothDevice(QBluetoothDeviceInfo device, QObject *parent = nullptr);

  private:
    QString m_address;
    QString m_name;
};

#endif // BLUETOOTHDEVICE_Hs