// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <QKeyEvent>
#include <QListWidgetItem>
#include <QScopedPointer>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>
#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtBluetooth/QBluetoothLocalDevice>

#include "bluetoothdto.h"

class BluetoothManager : public QObject
{
    Q_OBJECT

  public:
    BluetoothManager(QObject *parent);
    ~BluetoothManager();

  signals:
    void deviceDiscovered(const BluetoothDevice &device);

  public slots:
    void startScan();
    // void pairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing);
    // void pairingConfirm(const QBluetoothAddress &address, QString pin);

  private slots:
    // void scanFinished();
    // void pairDevice();
    // void unpairDevice();
    // void keyPressEvent(QKeyEvent *event);
    void onDeviceDiscovered(QBluetoothDeviceInfo device);

  private:
    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothLocalDevice *localDevice;
};

#endif // BLUETOOTHMANAGER_H
