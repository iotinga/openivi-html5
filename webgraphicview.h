#ifndef OPENIVI_WEBGRAPHICVIEW_H_
#define OPENIVI_WEBGRAPHICVIEW_H_

/*
    OpenIVI HTML5 environment
    Copyright (C) 2016 ATS Advanced Telematic Systems GmbH

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

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsWebView>
#include <QWebInspector>

#include "softwareloadingmanager.h"
#include "car.h"
#include "virtualkeyboard.h"
#include "allowlocation_webpage.h"

// #define USE_VIRTUAL_KEYBOARD

class WebGraphicView : public QGraphicsView {
  Q_OBJECT

 public:
  explicit WebGraphicView(QWidget *parent = 0);
  virtual ~WebGraphicView();

  // overriden from QGraphicsView
  virtual void resizeEvent(QResizeEvent *event);
  void SetUrl(const QUrl &url);

  void AddSettings(const QString& settingFilePath);
  void SetInputMode(DataInputMode inputMode);
 public slots:
  void FocusUpdate();

 private slots:
  void AddJavascriptObjectsToWindow();

 private:
  AllowLocationWebPage *page_;
  QGraphicsWebView *view_;
  QGraphicsScene *scene_;
  QWebInspector *webInspector_;
  SoftwareLoadingManager *softwareLoadingManager_;
#ifdef USE_VIRTUAL_KEYBOARD
  bool keyboardVisible_;
  VirtualKeyboard virtualKeyboard_;
#endif
  Car *car_;
};

/* vim: set expandtab tabstop=2 shiftwidth=2: */
#endif  // OPENIVI_WEBGRAPHICVIEW_H_
