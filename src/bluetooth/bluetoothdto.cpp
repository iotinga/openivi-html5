
#include "bluetoothdto.h"

BluetoothDevice::BluetoothDevice(QBluetoothDeviceInfo device, QObject *parent)
    : QObject(parent), m_address(device.address().toString()), m_name(device.name())
{}