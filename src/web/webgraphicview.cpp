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

#include "web/webgraphicview.h"

#include <QWebFrame>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusMessage>

#include "softwareloadingmanager.h"
#include "web/webpage.h"

WebGraphicView::WebGraphicView(QWidget *parent)
    : QGraphicsView(parent), page_(new WebPage(this)), view_(new QGraphicsWebView), scene_(new QGraphicsScene(this)),
      softwareLoadingManager_(new SoftwareLoadingManager(this)), phone_(new Phone(this)),
      script_(new ScriptRunner(this)), bluetoothManager_(new BluetoothManager(this))
{
    page_->settings()->setAttribute(QWebSettings::DeveloperExtrasEnabled, false);
    page_->settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, false);

    page_->settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);
    connect(page_->mainFrame(), SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(AddJavascriptObjectsToWindow()));
    view_->setPage(page_);

    setScene(scene_);
    scene_->addItem(view_);
    connect(phone_, SIGNAL(save_phone_file(const QString &)), this, SLOT(onSavePhoneFile(const QString &)));
}

WebGraphicView::~WebGraphicView()
{
    delete view_;
}

void WebGraphicView::resizeEvent(QResizeEvent *event)
{
    view_->resize(this->size());
    QGraphicsView::resizeEvent(event);
}

void WebGraphicView::SetUrl(const QUrl &url)
{
    view_->setUrl(url);
}

void WebGraphicView::AddJavascriptObjectsToWindow()
{
    page_->currentFrame()->addToJavaScriptWindowObject("slm", softwareLoadingManager_);
    page_->currentFrame()->evaluateJavaScript("genivi = {slm:slm}");
    page_->currentFrame()->evaluateJavaScript("navigator.geolocation = {"
                                              "  watchPosition: function () {},"
                                              "  getCurrentPosition: function () {}"
                                              "};");
    page_->currentFrame()->addToJavaScriptWindowObject("phone", phone_);
    page_->currentFrame()->addToJavaScriptWindowObject("script", script_);
    page_->currentFrame()->addToJavaScriptWindowObject("bluetoothManager", bluetoothManager_);
    page_->currentFrame()->addToJavaScriptWindowObject("controller", this);
}

void WebGraphicView::keyPressEvent(QKeyEvent *event)
{
    qDebug() << "Key pressed: " << event->key();
    phone_->setLastKey(event->key());
    phone_->key_pressed(event->key());
}

void WebGraphicView::OnOpenCameraView()
{
    script_->SetScriptPath("open");
    script_->OnRun("/home/kaskeeeee/repo/openivi-html5/ofono");
}

void WebGraphicView::onSavePhoneFile(const QString &content)
{
    QUrl viewUrl = view_->url();

    if (viewUrl.isLocalFile())
    {
        QString phoneIndexName = viewUrl.path();
        phoneIndexName.replace("index.html", "phoneIndex.js");
        QFile file(phoneIndexName);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            return;
        }
        QTextStream out(&file);
        out << content;
        file.close();
    }
}

void WebGraphicView::SetScriptPath(const QString &runScriptPath)
{
    script_->SetScriptPath(runScriptPath);
}

/* vim: set expandtab tabstop=2 shiftwidth=2: */
