// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "bluetoothmanager.h"
#ifdef DASH_LAYOUT_L5
#include "ui_bluetoothmanager_l5.h"
#endif
#ifdef DASH_LAYOUT_P5
#include "ui_bluetoothmanager_p5.h"
#endif
#ifdef DASH_LAYOUT_L6
#include "ui_bluetoothmanager_l6.h"
#endif
#ifdef DASH_LAYOUT_P6
#include "ui_bluetoothmanager_p6.h"
#endif

#include <QDebug>

#include "bluez_deviceinterface.h"

#define BLUEZ_BASE_OBJECT_PATH "/org/bluez/"
#define LOCAL_INTERFACE_DEVICE_NAME "hci0"

// Map key presses to input events in interface
#define INPUT_KEY_PLUS_PC   0x01000014
#define INPUT_KEY_MINUS_PC  0x01000012
#define INPUT_KEY_ENTER_PC  0x01000004
#define INPUT_KEY_BACK_PC   0x01000003
#define INPUT_KEY_PLUS_MD   0x72
#define INPUT_KEY_MINUS_MD  0x71
#define INPUT_KEY_ENTER_MD  0x24
#define INPUT_KEY_BACK_MD   0x16

BluetoothManager::BluetoothManager(QWidget* parent)
    : QDialog(parent), localDevice(new QBluetoothLocalDevice), m_ui(new Ui::BluetoothManager)
{
    m_ui->setupUi(this);

    // Apply stylesheet from resources
    QFile file(":/help/bluetooth.css");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    setStyleSheet(styleSheet);
    file.close();

    // When starting, always power on local device if necessary
    if (localDevice->hostMode() == QBluetoothLocalDevice::HostPoweredOff) {
        localDevice->powerOn();
    }
    discoveryAgent = new QBluetoothDeviceDiscoveryAgent();

    connect(m_ui->scanButton, SIGNAL(clicked()), this, SLOT(startScan()));
    connect(m_ui->pairButton, SIGNAL(clicked()), this, SLOT(pairDevice()));
    connect(m_ui->unpairButton, SIGNAL(clicked()), this, SLOT(unpairDevice()));
    connect(m_ui->closeButton, SIGNAL(clicked()), this, SLOT(close()));
    connect(discoveryAgent, SIGNAL(deviceDiscovered(QBluetoothDeviceInfo)),
            this, SLOT(addDevice(QBluetoothDeviceInfo)));
    connect(discoveryAgent, SIGNAL(finished()), this, SLOT(scanFinished()));
    connect(m_ui->listWidget, SIGNAL(itemActivated(QListWidgetItem*)), this, SLOT(bluetoothDeviceSelected(QListWidgetItem*)));
    connect(m_ui->listWidget, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(bluetoothDeviceSelected(QListWidgetItem*)));
    connect(localDevice, SIGNAL(deviceConnected(const QBluetoothAddress &)), this, SLOT(onDeviceConnected(const QBluetoothAddress &)));
    connect(localDevice, SIGNAL(deviceDisconnected(const QBluetoothAddress &)), this, SLOT(onDeviceDisconnected(const QBluetoothAddress &)));
    connect(localDevice, SIGNAL(pairingFinished(QBluetoothAddress,QBluetoothLocalDevice::Pairing)), this, SLOT(pairingDone(QBluetoothAddress,QBluetoothLocalDevice::Pairing)));
    connect(localDevice, SIGNAL(pairingDisplayConfirmation(const QBluetoothAddress&, QString)), this, SLOT(pairingConfirm(const QBluetoothAddress&, QString)));

    grabKeyboard();
}

BluetoothManager::~BluetoothManager()
{
    releaseKeyboard();
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
        // Set background if device is connected
        QList<QBluetoothAddress> connectedDevices = localDevice->connectedDevices();
        if (connectedDevices.indexOf(btDevInfo.address()) >= 0) {
            item->setBackground(QColor(110, 170, 255));
        }

        m_ui->listWidget->addItem(item);
    }
}

void BluetoothManager::startScan()
{
    if (discoveryAgent->isActive()) {
        // Stop scanning
        discoveryAgent->stop();
        m_ui->scanButton->setText("Scan");
        m_ui->infoLabel->setText("");
    } else {
        m_ui->listWidget->clear();
        discoveryAgent->start();
        m_ui->scanButton->setText("Stop scanning");
        m_ui->infoLabel->setText("Scan in progress...");
        m_ui->pairButton->setEnabled(false);
        m_ui->unpairButton->setEnabled(false);
    }
    // m_ui->scanButton->setEnabled(false);
}

void BluetoothManager::scanFinished()
{
    m_ui->scanButton->setText("Scan");
    m_ui->infoLabel->setText("");
}

void BluetoothManager::bluetoothDeviceSelected(QListWidgetItem* item)
{
    QString deviceText = item->text();

    selectedDeviceName = getDeviceName(deviceText);
    selectedDeviceAddress = getDeviceAddress(deviceText);

    qDebug() << "Selected bluetooth device (" << selectedDeviceName << ") [" << selectedDeviceAddress.toString() << "]";


    if (localDevice->pairingStatus(selectedDeviceAddress) == QBluetoothLocalDevice::Unpaired) {
        // Device not paired, only enable pairing button
        m_ui->pairButton->setEnabled(true);
        m_ui->unpairButton->setEnabled(false);
        m_ui->pairButton->setText("Pair");
    } else {
        QList<QBluetoothAddress> connectedDevices = localDevice->connectedDevices();
        if (connectedDevices.indexOf(selectedDeviceAddress) >= 0) {
            // Device is already connected, can only disconnect
            m_ui->pairButton->setEnabled(false);
            m_ui->unpairButton->setEnabled(true);
            m_ui->pairButton->setText("Connect");
            m_ui->unpairButton->setText("Disconnect");
        } else {
            // Device is paired, but not connected already connected
            // can forget or connect
            m_ui->pairButton->setEnabled(true);
            m_ui->unpairButton->setEnabled(true);
            m_ui->pairButton->setText("Connect");
            m_ui->unpairButton->setText("Forget pairing");
        }
    }

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
    OrgBluezDevice1Interface* bluezDeviceInterface = NULL;
    QString deviceObjectPath = BLUEZ_BASE_OBJECT_PATH;

    if (selectedDeviceName.isEmpty() || selectedDeviceAddress.isNull()) {
        // Invalid selection, return
        return;
    }

    if (localDevice->pairingStatus(selectedDeviceAddress) == QBluetoothLocalDevice::Unpaired) {
        // Using AuthorizedPaired automatically trusts device
        // after pairing (seems to depend on BlueZ or Qt version)
        // localDevice->requestPairing(selectedDeviceAddress, QBluetoothLocalDevice::Paired);
        localDevice->requestPairing(selectedDeviceAddress, QBluetoothLocalDevice::AuthorizedPaired);
    } else {
        deviceObjectPath.append(LOCAL_INTERFACE_DEVICE_NAME);
        deviceObjectPath.append("/dev_");
        deviceObjectPath.append(selectedDeviceAddress.toString().replace(":", "_"));
        bluezDeviceInterface = new OrgBluezDevice1Interface("org.bluez", deviceObjectPath, QDBusConnection::systemBus());
        if (!bluezDeviceInterface->connected()) {
            bluezDeviceInterface->Connect();
        }
        delete bluezDeviceInterface;
    }
}

void BluetoothManager::unpairDevice()
{
    OrgBluezDevice1Interface* bluezDeviceInterface = NULL;
    QString deviceObjectPath = "/org/bluez/";

    if (selectedDeviceName.isEmpty() || selectedDeviceAddress.isNull()) {
        // Invalid selection, return
        return;
    }

    if (localDevice->pairingStatus(selectedDeviceAddress) != QBluetoothLocalDevice::Unpaired) {
        localDevice->requestPairing(selectedDeviceAddress, QBluetoothLocalDevice::Unpaired);
    } else {
        deviceObjectPath.append(LOCAL_INTERFACE_DEVICE_NAME);
        deviceObjectPath.append("/dev_");
        deviceObjectPath.append(selectedDeviceAddress.toString().replace(":", "_"));
        bluezDeviceInterface = new OrgBluezDevice1Interface("org.bluez", deviceObjectPath, QDBusConnection::systemBus());
        if (bluezDeviceInterface->connected()) {
            bluezDeviceInterface->Disconnect();
        }
        delete bluezDeviceInterface;
    }
}

void BluetoothManager::pairingConfirm(const QBluetoothAddress& address, QString pin)
{
    qDebug() << "Pairing device " << address.toString() << " with passcode " << pin;

    localDevice->pairingConfirmation(true);
}


void BluetoothManager::pairingDone(const QBluetoothAddress& address, QBluetoothLocalDevice::Pairing pairing)
{
    OrgBluezDevice1Interface* bluezDeviceInterface = NULL;
    QString deviceObjectPath = BLUEZ_BASE_OBJECT_PATH;

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
        // This forces to refresh the buttons
        if (items.at(var) == m_ui->listWidget->currentItem()) {
            bluetoothDeviceSelected(items.at(var));
        }
    }

    // Set the device as trusted
    deviceObjectPath.append(LOCAL_INTERFACE_DEVICE_NAME);
    deviceObjectPath.append("/dev_");
    deviceObjectPath.append(address.toString().replace(":", "_"));
    bluezDeviceInterface = new OrgBluezDevice1Interface("org.bluez", deviceObjectPath, QDBusConnection::systemBus());
    if (!bluezDeviceInterface->trusted()) {
        bluezDeviceInterface->setTrusted(true);
    }
    delete bluezDeviceInterface;

}


void BluetoothManager::onDeviceConnected(const QBluetoothAddress& address)
{
    qDebug() << "Device connected " << address.toString();
    QList<QListWidgetItem *> items = m_ui->listWidget->findItems(address.toString(), Qt::MatchContains);
    if (!items.empty()) {
        for (int dd = 0; dd < items.size(); ++dd) {
            items.at(dd)->setBackground(QColor(110, 170, 255));
            // This forces to refresh the buttons
            if (items.at(dd) == m_ui->listWidget->currentItem()) {
                bluetoothDeviceSelected(items.at(dd));
            }

        }
    }
}

void BluetoothManager::onDeviceDisconnected(const QBluetoothAddress& address)
{
    qDebug() << "Device disconnected " << address.toString();
    QList<QListWidgetItem *> items = m_ui->listWidget->findItems(address.toString(), Qt::MatchContains);
    if (!items.empty()) {
        for (int dd = 0; dd < items.size(); ++dd) {
            items.at(dd)->setBackground(Qt::white);
            // This forces to refresh the buttons
            if (items.at(dd) == m_ui->listWidget->currentItem()) {
                bluetoothDeviceSelected(items.at(dd));
            }
        }
    }
}

void BluetoothManager::keyPressEvent(QKeyEvent *event)
{
#ifdef ENABLE_ADVANCED_DEBUG
  qDebug() << "Key pressed: " << event->key();
#endif

  // Debug
  /* QString debugMsg("Last Key pressed %1");
  debugMsg = debugMsg.arg(event->key());
  m_ui->infoLabel->setText(debugMsg); */

  int currRow = 0;
  QListWidgetItem* deviceItem = NULL;

  switch (event->key()) {
      case INPUT_KEY_MINUS_PC:
      case INPUT_KEY_MINUS_MD:
      case Qt::Key_Down: {
        if (m_ui->listWidget->hasFocus()) {
            deviceItem = m_ui->listWidget->currentItem();
            currRow = m_ui->listWidget->row(deviceItem);
            qDebug() << "Row " << currRow << "/" << m_ui->listWidget->count();
            if (currRow < m_ui->listWidget->count() - 1) {
                currRow++;
                deviceItem = m_ui->listWidget->item(currRow);
                m_ui->listWidget->setCurrentItem(deviceItem);
            } else {
                focusNextChild();
            }
        } else {
            focusNextChild();
        }
      } break;
      case INPUT_KEY_PLUS_PC:
      case INPUT_KEY_PLUS_MD:
      case Qt::Key_Up: {
          if (m_ui->listWidget->hasFocus()) {
            deviceItem = m_ui->listWidget->currentItem();
            currRow = m_ui->listWidget->row(deviceItem);
            qDebug() << "Row " << currRow << "/" << m_ui->listWidget->count();
            if (currRow > 0) {
                currRow--;
                deviceItem = m_ui->listWidget->item(currRow);
                m_ui->listWidget->setCurrentItem(deviceItem);
            } else {
                focusPreviousChild();
            }
          } else {
              focusPreviousChild();
          }
      } break;
      case INPUT_KEY_ENTER_MD:
      case Qt::Key_Return: {
          if (m_ui->listWidget->hasFocus()) {
            deviceItem = m_ui->listWidget->currentItem();
            if (deviceItem != NULL) {
                bluetoothDeviceSelected(deviceItem);
            }
          } else {
            QWidget* currentFocusWidget = focusWidget();
            QPushButton* button = qobject_cast<QPushButton*>(currentFocusWidget);
            if (button != NULL) {
                // Push button
                button->click();
            } else {
                close();
            }
          }
        } break;
      case INPUT_KEY_BACK_MD:
      case Qt::Key_Backspace: {
          if (m_ui->listWidget->hasFocus()) {
            focusNextChild();
          }
      } break;
      default:
          break;
  }
}
