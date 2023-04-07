// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "bluetoothmanager.h"
#include "ui_bluetoothmanager.h"

#include <QDebug>

BluetoothManager::BluetoothManager(QWidget* parent)
    : QDialog(parent), localDevice(new QBluetoothLocalDevice), m_ui(new Ui::BluetoothManager)
{
    m_ui->setupUi(this);

    // When starting, always power on local device if necessary
    if (localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
        localDevice->powerOn();
    }
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();

    connect(m_ui->scanButton, SIGNAL(clicked()), this, SLOT(startScan()));
    connect(m_ui->pairButton, SIGNAL(clicked()), this, SLOT(pairDevice()));
    connect(m_ui->unpairButton, SIGNAL(clicked()), this, SLOT(unpairDevice()));
    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(addDevice(QBluetoothDeviceInfo)));
    connect(discoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));
    connect(m_ui->listWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(bluetoothDeviceSelected(QListWidgetItem*)));
    connect(localDevice, SIGNAL(deviceConnected(const QBluetoothAddress &)), this, SLOT(onDeviceConnected(const QBluetoothAddress &)));
    connect(localDevice, SIGNAL(deviceDisconnected(const QBluetoothAddress &)), this, SLOT(onDeviceDisconnected(const QBluetoothAddress &)));
    connect(localDevice, SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing)), this, SLOT(pairingDone(QBluetoothAddress,QBluetoothLocalDevice::Pairing)));
}

BluetoothManager::~BluetoothManager()
{
    delete discoveryAgent;
}

void BluetoothManager::addDevice(const QBluetoothDeviceInfo& btDevInfo)
{
    QString label = QString("%1 (MAC: %2)").arg(btDevInfo.name()).arg(btDevInfo.address().toString());
    QList<QListWidgetItem *> items = m_ui->listWidget->findItems(label, Qt::MatchExactly);
    if (items.empty()) {
        QListWidgetItem *item = new QListWidgetItem(label);
        QBluetoothLocalDevice::Pairing pairingStatus = localDevice->pairingStatus(btDevInfo.address());
        QColor devPairColor;
        switch (pairingStatus) {
            case QBluetoothLocalDevice::Paired:
                devPairColor = QColor(Qt::darkBlue);
                break;
            case QBluetoothLocalDevice::AuthorizedPaired:
                devPairColor = QColor(Qt::darkGreen);
                break;
            default:
                devPairColor = QColor(Qt::black);
                break;
        }
        item->setForeground(devPairColor);

        m_ui->listWidget->addItem(item);
    }
}

void BluetoothManager::startScan()
{
    discoveryAgent->start();
    m_ui->scanButton->setEnabled(false);
    m_ui->listWidget->clear();
    m_ui->infoLabel->setText("Scan in progress...");
}

void BluetoothManager::scanFinished()
{
    m_ui->scanButton->setEnabled(true);
    m_ui->infoLabel->setText("");
}

void BluetoothManager::bluetoothDeviceSelected(QListWidgetItem* item)
{
    QString deviceText = item->text();

    selectedDeviceName = getDeviceName(deviceText);
    selectedDeviceAddress = getDeviceAddress(deviceText);

    qDebug() << "Selected bluetooth device (" << selectedDeviceName << ") [" << selectedDeviceAddress.toString() << "]";
}

QString BluetoothManager::getDeviceName(QString displayString)
{
    QString deviceName;

    int pos = displayString.indexOf("(MAC:");
    if (pos != -1) {
        deviceName = displayString.left(pos - 1);
    }

    return deviceName;
}

QBluetoothAddress BluetoothManager::getDeviceAddress(QString displayString)
{
    QString addressString;

    int pos = displayString.indexOf("(MAC:");
    if (pos != -1) {
        addressString = displayString.mid(pos + 6);
        addressString.chop(1);
    }

    QBluetoothAddress address(addressString);
    return address;
}

void BluetoothManager::pairDevice()
{
    if (selectedDeviceName.isEmpty() || selectedDeviceAddress.isNull()) {
        // Invalid selection, return
        return;
    }

    localDevice->requestPairing(selectedDeviceAddress, QBluetoothLocalDevice::Paired);
}

void BluetoothManager::unpairDevice()
{
    if (selectedDeviceName.isEmpty() || selectedDeviceAddress.isNull()) {
        // Invalid selection, return
        return;
    }

    localDevice->requestPairing(selectedDeviceAddress, QBluetoothLocalDevice::Unpaired);
}

void BluetoothManager::pairingDone(const QBluetoothAddress& address, QBluetoothLocalDevice::Pairing pairing)
{
    QList<QListWidgetItem *> items = m_ui->listWidget->findItems(address.toString(), Qt::MatchContains);

    QColor devPairColor;
    switch (pairing) {
        case QBluetoothLocalDevice::Paired:
            devPairColor = QColor(Qt::darkBlue);
            break;
        case QBluetoothLocalDevice::AuthorizedPaired:
            devPairColor = QColor(Qt::darkGreen);
            break;
        default:
            devPairColor = QColor(Qt::black);
            break;
    }
    for (int var = 0; var < items.count(); ++var) {
        QListWidgetItem *item = items.at(var);
        item->setForeground(devPairColor);
    }
}


void BluetoothManager::onDeviceConnected(const QBluetoothAddress& address)
{
    QList<QListWidgetItem *> items = m_ui->listWidget->findItems(address.toString(), Qt::MatchContains);
    if (!items.empty()) {
        for (qsizetype dd = 0; dd < items.size(); ++dd) {
            items.at(dd)->setBackground(QColor(110, 170, 255));
        }
    }
}

void BluetoothManager::onDeviceDisconnected(const QBluetoothAddress& address)
{
    QList<QListWidgetItem *> items = m_ui->listWidget->findItems(address.toString(), Qt::MatchContains);
    if (!items.empty()) {
        for (qsizetype dd = 0; dd < items.size(); ++dd) {
            items.at(dd)->setBackground(Qt::white);
        }
    }
}
