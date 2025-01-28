#include "allowlocation_webpage.h"

#include <QDebug>

AllowLocationWebPage::AllowLocationWebPage(QObject *parent) : QWebPage(parent)
{
    connect(this, SIGNAL(featurePermissionRequested(QWebFrame *, QWebPage::Feature)),
            SLOT(permissionRequested(QWebFrame *, QWebPage::Feature)));
}

void AllowLocationWebPage::permissionRequested(QWebFrame *frame, QWebPage::Feature feature)
{
    if (feature == Geolocation)
    {
        setFeaturePermission(frame, feature, PermissionGrantedByUser);
    }
}

void AllowLocationWebPage::javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID)
{
    qDebug() << "JSLog [" << sourceID << "," << lineNumber << "]:" << message;
}
