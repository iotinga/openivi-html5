#pragma once

#include <QWebPage>

class WebPage : public QWebPage
{
    Q_OBJECT

  public:
    explicit WebPage(QObject *parent = Q_NULLPTR);

  protected:
    void javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID) override;
};