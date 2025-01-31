// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "bluetoothmanager.h"

#include <QDebug>

#include "bluez_deviceinterface.h"

#define BLUEZ_BASE_OBJECT_PATH      "/org/bluez/"
#define LOCAL_INTERFACE_DEVICE_NAME "hci0"

BluetoothManager::BluetoothManager(QObject *parent) : QObject(parent), localDevice(new QBluetoothLocalDevice)
{

    // When starting, always power on local device if necessary
    if (localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff)
    {
        localDevice->powerOn();
    }

    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();
    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)), this,
            SLOT(onDeviceDiscovered(QBluetoothDeviceInfo)));
    // connect(discoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));
    // connect(localDevice, SIGNAL(deviceConnected(const QBluetoothAddress &)), this,
    //         SLOT(onDeviceConnected(const QBluetoothAddress &)));
    // connect(localDevice, SIGNAL(deviceDisconnected(const QBluetoothAddress &)), this,
    //         SLOT(onDeviceDisconnected(const QBluetoothAddress &)));
    // connect(localDevice, SIGNAL(pairingFinished(QBluetoothAddress, QBluetoothLocalDevice::Pairing)), this,
    //         SLOT(pairingDone(QBluetoothAddress, QBluetoothLocalDevice::Pairing)));
    // connect(localDevice, SIGNAL(pairingDisplayConfirmation(const QBluetoothAddress &, QString)), this,
    //         SLOT(pairingConfirm(const QBluetoothAddress &, QString)));
}

BluetoothManager::~BluetoothManager()
{
    delete discoveryAgent;
}

void BluetoothManager::onDeviceDiscovered(QBluetoothDeviceInfo device)
{
    BluetoothDevice dto(device, nullptr);
    deviceDiscovered(dto);
}

void BluetoothManager::startScan()
{
    discoveryAgent->start();
}

// void BluetoothManager::pairDevice()
// {
// OrgBluezDevice1Interface *bluezDeviceInterface = NULL;
// QString deviceObjectPath = BLUEZ_BASE_OBJECT_PATH;

// if (selectedDeviceName.isEmpty() || selectedDeviceAddress.isNull())
// {
//     // Invalid selection, return
//     return;
// }

// if (localDevice->pairingStatus(selectedDeviceAddress) == QBluetoothLocalDevice::Unpaired)
// {
//     // Using AuthorizedPaired automatically trusts device
//     // after pairing (seems to depend on BlueZ or Qt version)
//     // localDevice->requestPairing(selectedDeviceAddress, QBluetoothLocalDevice::Paired);
//     localDevice->requestPairing(selectedDeviceAddress, QBluetoothLocalDevice::AuthorizedPaired);
// }
// else
// {
//     deviceObjectPath.append(LOCAL_INTERFACE_DEVICE_NAME);
//     deviceObjectPath.append("/dev_");
//     deviceObjectPath.append(selectedDeviceAddress.toString().replace(":", "_"));
//     bluezDeviceInterface =
//         new OrgBluezDevice1Interface("org.bluez", deviceObjectPath, QDBusConnection::systemBus());
//     if (!bluezDeviceInterface->connected())
//     {
//         bluezDeviceInterface->Connect();
//     }
//     delete bluezDeviceInterface;
// }
// }

// void BluetoothManager::unpairDevice()
// {
// OrgBluezDevice1Interface *bluezDeviceInterface = NULL;
// QString deviceObjectPath = "/org/bluez/";

// if (selectedDeviceName.isEmpty() || selectedDeviceAddress.isNull())
// {
//     // Invalid selection, return
//     return;
// }

// if (localDevice->pairingStatus(selectedDeviceAddress) != QBluetoothLocalDevice::Unpaired)
// {
//     localDevice->requestPairing(selectedDeviceAddress, QBluetoothLocalDevice::Unpaired);
// }
// else
// {
//     deviceObjectPath.append(LOCAL_INTERFACE_DEVICE_NAME);
//     deviceObjectPath.append("/dev_");
//     deviceObjectPath.append(selectedDeviceAddress.toString().replace(":", "_"));
//     bluezDeviceInterface =
//         new OrgBluezDevice1Interface("org.bluez", deviceObjectPath, QDBusConnection::systemBus());
//     if (bluezDeviceInterface->connected())
//     {
//         bluezDeviceInterface->Disconnect();
//     }
//     delete bluezDeviceInterface;
// }
// }

// void BluetoothManager::pairingConfirm(const QBluetoothAddress &address, QString pin)
// {
// qDebug() << "Pairing device " << address.toString() << " with passcode " << pin;
// localDevice->pairingConfirmation(true);
// }

// void BluetoothManager::pairingDone(const QBluetoothAddress &address, QBluetoothLocalDevice::Pairing pairing)
// {
// OrgBluezDevice1Interface *bluezDeviceInterface = NULL;
// QString deviceObjectPath = BLUEZ_BASE_OBJECT_PATH;

// QList<QListWidgetItem *> items = m_ui->listWidget->findItems(address.toString(), Qt::MatchContains);

// QColor devPairColor;
// switch (pairing)
// {
// case QBluetoothLocalDevice::Paired:
//     devPairColor = QColor(Qt::darkBlue);
//     break;
// case QBluetoothLocalDevice::AuthorizedPaired:
//     devPairColor = QColor(Qt::darkGreen);
//     break;
// default:
//     devPairColor = QColor(Qt::black);
//     break;
// }
// for (int var = 0; var < items.count(); ++var)
// {
//     QListWidgetItem *item = items.at(var);
//     item->setForeground(devPairColor);
//     // This forces to refresh the buttons
//     if (items.at(var) == m_ui->listWidget->currentItem())
//     {
//         bluetoothDeviceSelected(items.at(var));
//     }
// }

// // Set the device as trusted
// deviceObjectPath.append(LOCAL_INTERFACE_DEVICE_NAME);
// deviceObjectPath.append("/dev_");
// deviceObjectPath.append(address.toString().replace(":", "_"));
// bluezDeviceInterface = new OrgBluezDevice1Interface("org.bluez", deviceObjectPath, QDBusConnection::systemBus());
// if (!bluezDeviceInterface->trusted())
// {
//     bluezDeviceInterface->setTrusted(true);
// }
// delete bluezDeviceInterface;
// }