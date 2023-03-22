// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef PHONE_H
#define PHONE_H

#include <QObject>

// Include ofono classes generated from D-Bus interfaces
#include "ofono/OfonoManager.h"
#include "ofono/OfonoVoiceCallManager.h"
#include "ofono/OfonoVoiceCall.h"
#include "ofono/OfonoNetworkRegistration.h"
#include "ofono/OfonoHandsfree.h"
#include "ofono/OfonoCallVolume.h"

/**
 * @todo write docs
 */
class Phone : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name MEMBER m_name);
    Q_PROPERTY(QString status MEMBER currentCallStatus);
    Q_PROPERTY(QString number MEMBER currentCallNumber);
    Q_PROPERTY(int signal MEMBER m_signal);
    Q_PROPERTY(int battery MEMBER m_battery);
    Q_PROPERTY(int volume MEMBER m_volume);
public:
    /**
     * Default constructor
     */
    Phone(QObject *parent = 0);

    /**
     * Destructor
     */
    ~Phone();

    /** Init ofono interface and enumerate modems. */
    void InitOfono();
    /** Init one ofono modem to enable interface calls */
    void InitModem(OfonoModem& modemInfo);

private slots:
    void OnCall(const QString& phoneNumber);
    void OnHangup();
    void OnCallStarted(const QDBusObjectPath &path, const QVariantMap &properties);
    void OnCallClosed(const QDBusObjectPath &path);
    void OnCallPropertyChanged(const QString &propertyName, const QDBusVariant &propertyValue);
    void OnNetworkInfoChanged(const QString &propertyName, const QDBusVariant &propertyValue);
    void OnHandsfreePropertyChanged(const QString &propertyName, const QDBusVariant &propertyValue);
    void OnVolumePropertyChanged(const QString &propertyName, const QDBusVariant &propertyValue);

private:
    // Ofono general interfaces
    OrgOfonoManagerInterface *ofonoInterface;
    OrgOfonoVoiceCallManagerInterface *ofonoVoiceCallManager;
    OrgOfonoNetworkRegistrationInterface *ofonoNetworkInfo;
    OrgOfonoHandsfreeInterface *ofonoHandsfreeInfo;
    OrgOfonoCallVolumeInterface *ofonoVolume;

    // Voice call objects
    OrgOfonoVoiceCallInterface *ofonoVoiceCall;
    QDBusObjectPath currentCallPath;

    // Exported properties
    QString currentCallNumber;
    QString currentCallStatus;
    QString m_name;
    int m_signal;
    int m_battery;
    int m_volume;

};

#endif // PHONE_H
