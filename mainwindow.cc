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

#include "mainwindow.h"
#ifdef DASH_LAYOUT_L5
#include "ui_mainwindow_l5.h"
#endif
#ifdef DASH_LAYOUT_P5
#include "ui_mainwindow_p5.h"
#endif
#ifdef DASH_LAYOUT_L6
#include "ui_mainwindow_l6.h"
#endif
#ifdef DASH_LAYOUT_P6
#include "ui_mainwindow_p6.h"
#endif
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

  if (force_url.isEmpty()) {
    QSettings settings;
    QUrl startingUrl =
        settings.value("homepage", "qrc:/help/welcome.html").toUrl();
    SetUrl(startingUrl);
  } else {
    SetUrl(force_url);
  }
}

MainWindow::~MainWindow() { delete ui_; }

void MainWindow::SetUrl(const QUrl &url) { ui_->graphicsView->SetUrl(url); }

void MainWindow::OpenUrlDialog() {
  QUrl s = QFileDialog::getOpenFileUrl(this, "Choose a URL to open");
  if (!s.isEmpty()) {
    QSettings settings;
    settings.setValue("homepage", s);
    SetUrl(s);
  }
}

void MainWindow::ToggleFullScreen() {
  bool shownhide;
  if (isFullScreen()) {
    setWindowState(Qt::WindowNoState);
    shownhide = true;
  } else {
    setWindowState(Qt::WindowFullScreen);
    shownhide = false;
  }
  if (shownhide) {
    ui_->menuBar_->show();
    ui_->statusBar_->show();
  } else {
    ui_->menuBar_->hide();
    ui_->statusBar_->hide();
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
