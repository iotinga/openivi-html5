// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "bluetoothmanager.h"
#include "ui_bluetoothmanager.h"

BluetoothManager::~BluetoothManager()
{
}

BluetoothManager::BluetoothManager(QWidget* parent)
    : QDialog(parent), m_ui(new Ui::BluetoothManager)
{
    m_ui->setupUi(this);
}
