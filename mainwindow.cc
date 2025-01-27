/*
    OpenIVI HTML5 environment
    Copyright (C) 2015 ATS Advanced Telematic Systems GmbH

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include <QFileDialog>
#include <QSettings>
#include <bluetooth/bluetoothmanager.h>

MainWindow::MainWindow(QWidget *parent, const QUrl &force_url, DataInputMode inputMode, const QString &settingsPath)
    : QMainWindow(parent), ui_(new Ui::MainWindow) {
  QWebSettings::globalSettings()->enablePersistentStorage();

  ui_->setupUi(this);
  // btManager = NULL;
  // Menu behaviours
  connect(ui_->actionOpen_URL, SIGNAL(triggered()), this,
          SLOT(OpenUrlDialog()));
  connect(ui_->action_Quit, SIGNAL(triggered()), qApp, SLOT(quit()));

  connect(ui_->action_Full_Screen, SIGNAL(triggered()), this,
          SLOT(ToggleFullScreen()));

  connect(ui_->actionBluetooth, SIGNAL(triggered()), this, SLOT(ManageBluetooth()));

  connect(ui_->graphicsView, SIGNAL(open_bluetooth_manager()), this, SLOT(ManageBluetooth()));

  /* IMPORTANT: always set settings BEFORE changing input mode.
   * This way, CAN bus settings will be parsed when CAN reader
   * is initialized. */
  if (!settingsPath.isEmpty()) {
    ui_->graphicsView->AddSettings(settingsPath);
  }

  // Select input source for cluster data
  ui_->graphicsView->SetInputMode(inputMode);

  QSettings settings;
  if (force_url.isEmpty()) {
    QUrl startingUrl =
        settings.value("homepage", "qrc:/home/index.html").toUrl();
    SetUrl(startingUrl);
  } else {
    SetUrl(force_url);
  }

  QString scriptPath = settings.value("script", "").toString();
  qDebug() << "Script path: " << scriptPath;
  ui_->graphicsView->SetScriptPath(scriptPath);
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::SetUrl(const QUrl &url) {
  ui_->graphicsView->SetUrl(url);
}

void MainWindow::SetScriptPath(const QString& scriptPath)
{
  qDebug() << "Script path: " << scriptPath;
  ui_->graphicsView->SetScriptPath(scriptPath);
}


void MainWindow::OpenUrlDialog() {
  QUrl s = QFileDialog::getOpenFileUrl(this, "Choose a URL to open");
  if (!s.isEmpty()) {
    QSettings settings;
    settings.setValue("homepage", s);
    SetUrl(s);
  }
}

void MainWindow::ToggleFullScreen() {
  if (isFullScreen()) {
    setWindowState(Qt::WindowNoState);
  } else {
    setWindowState(Qt::WindowFullScreen);
  }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Escape && isFullScreen()) {
    ToggleFullScreen();
  } else if (event->key() == Qt::Key_F11) {
    ToggleFullScreen();
  }
}

void MainWindow::ManageBluetooth()
{
  qDebug() << "Opening Bluetooth management Window";
//   if (btManager == NULL) {
//     btManager = new BluetoothManager(this);
//     btManager->setModal(true);
//     btManager->exec();
//   }
  BluetoothManager btManager(this);
  if (isFullScreen()) {
    btManager.setWindowState(Qt::WindowFullScreen);
  }
  btManager.exec();
}


/* vim: set expandtab tabstop=2 shiftwidth=2: */
