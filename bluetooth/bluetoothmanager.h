// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H

#include <QDialog>
#include <QScopedPointer>

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

private:
    QScopedPointer<Ui::BluetoothManager> m_ui;
};

#endif // BLUETOOTHMANAGER_H
