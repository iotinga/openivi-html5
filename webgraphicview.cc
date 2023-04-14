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

#include "webgraphicview.h"

#include <QWebFrame>
#include <QtDBus/QDBusMessage>
#include <QtDBus/QDBusConnection>
#include <QtWebKit/QtWebKit>

#include "softwareloadingmanager.h"

using org::genivi::software_loading_manager;

WebGraphicView::WebGraphicView(QWidget *parent)
    : QGraphicsView(parent),
      page_(new AllowLocationWebPage(this)),
      view_(new QGraphicsWebView),
      scene_(new QGraphicsScene(this)),
#ifdef _ADVANCED_DEBUG
      webInspector_(new QWebInspector),
#endif
      softwareLoadingManager_(new SoftwareLoadingManager(this)),
      car_(new Car(this)),
      phone_(new Phone(this)) {
#ifdef _ADVANCED_DEBUG
  page_->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
  page_->settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
#else
  page_->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, false);
  page_->settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, false);
  page_->setProperty("_q_webInspectorServerPort", 9221);
#endif
  page_->settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
  connect(page_->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this,
          SLOT(AddJavascriptObjectsToWindow()));
  view_->setPage(page_);

  setScene(scene_);
  scene_->addItem(view_);
#ifdef _ADVANCED_DEBUG
  webInspector_->setPage(page_);
#endif

#ifdef USE_VIRTUAL_KEYBOARD
  connect(page_, SIGNAL(microFocusChanged()), this, SLOT(FocusUpdate()));
#endif
  connect(phone_, SIGNAL(open_bluetooth_manager()), this, SLOT(OnOpenBluetoothManager()));
}

WebGraphicView::~WebGraphicView() {
  delete view_;
#ifdef _ADVANCED_DEBUG
  delete webInspector_;
#endif
}

void WebGraphicView::resizeEvent(QResizeEvent *event) {
  view_->resize(this->size());
  QGraphicsView::resizeEvent(event);
}

void WebGraphicView::SetUrl(const QUrl &url) { view_->setUrl(url); }

void WebGraphicView::FocusUpdate() {
#ifdef USE_VIRTUAL_KEYBOARD
  QVariant r = page_->inputMethodQuery(Qt::ImSurroundingText);
  bool shouldDisplayKeyboard = r.isValid();
  if (keyboardVisible_ != shouldDisplayKeyboard) {
    if (shouldDisplayKeyboard) {
      virtualKeyboard_.Show();
    } else {
      virtualKeyboard_.Hide();
    }
    keyboardVisible_ = shouldDisplayKeyboard;
  }
#endif
}

void WebGraphicView::AddJavascriptObjectsToWindow() {
  page_->currentFrame()->addToJavaScriptWindowObject("slm",
                                                     softwareLoadingManager_);

  page_->currentFrame()->evaluateJavaScript("genivi = {slm:slm}");

  page_->currentFrame()->evaluateJavaScript(
      "navigator.geolocation = {"
      "  watchPosition: function () {},"
      "  getCurrentPosition: function () {}"
      "};");

  page_->currentFrame()->addToJavaScriptWindowObject("car", car_);
  page_->currentFrame()->addToJavaScriptWindowObject("phone", phone_);
}

void WebGraphicView::AddSettings(const QString& settingFilePath)
{
  car_->ParseSettingFile(settingFilePath);
}


void WebGraphicView::SetInputMode(DataInputMode inputMode)
{
  car_->SetInputMode(inputMode);
}

void WebGraphicView::keyPressEvent(QKeyEvent *event)
{
  qDebug() << "Key pressed: " << event->key();
  phone_->setLastKey(event->key());
  phone_->key_pressed(event->key());
}

void WebGraphicView::OnOpenBluetoothManager()
{
  open_bluetooth_manager();
}


/* vim: set expandtab tabstop=2 shiftwidth=2: */
