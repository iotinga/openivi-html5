// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <QDialog>
#include <QScopedPointer>
#include <QListWidgetItem>
#include <QtBluetooth/QBluetoothLocalDevice>
#include <QtBluetooth/QBluetoothDeviceInfo>
#include <QtBluetooth/QBluetoothDeviceDiscoveryAgent>

namespace Ui
{
class BluetoothManager;
}

/**
 * @todo write docs
 */
class BluetoothManager : public QDialog
{
    Q_OBJECT

public:

    /**
     * Constructor
     *
     * @param parent TODO
     * @param f TODO
     */
    BluetoothManager(QWidget* parent);

    /** * Destructor */
    ~BluetoothManager();

public slots:
    void addDevice(const QBluetoothDeviceInfo& btDevInfo);
    void pairingDone(const QBluetoothAddress& address, QBluetoothLocalDevice::Pairing pairing);
    void pairingConfirm(const QBluetoothAddress &address, QString pin);

private slots:
    void startScan();
    void scanFinished();
    void bluetoothDeviceSelected(QListWidgetItem* item);
    void pairDevice();
    void unpairDevice();
    void onDeviceConnected(const QBluetoothAddress &address);
    void onDeviceDisconnected(const QBluetoothAddress &address);

private:
    QString getDeviceName(QString displayString);
    QBluetoothAddress getDeviceAddress(QString displayString);

    QBluetoothDeviceDiscoveryAgent *discoveryAgent;
    QBluetoothLocalDevice *localDevice;
    QScopedPointer<Ui::BluetoothManager> m_ui;

    QString selectedDeviceName;
    QBluetoothAddress selectedDeviceAddress;

};

#endif // BLUETOOTHMANAGER_H
