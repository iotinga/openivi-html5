// Manage Ofono interface via D-Bus to make phone calls.
// SPDX-FileCopyrightText: 2023 Davide Rondini <info@drondini.com>
// SPDX-License-Identifier: GPL-2.0-or-later

#ifndef PHONE_H
#define PHONE_H

#include <QObject>

// Include ofono classes generated from D-Bus interfaces
#include "ofono/OfonoCallVolume.h"
#include "ofono/OfonoHandsfree.h"
#include "ofono/OfonoManager.h"
#include "ofono/OfonoModem.h"
#include "ofono/OfonoNetworkRegistration.h"
#include "ofono/OfonoVoiceCall.h"
#include "ofono/OfonoVoiceCallManager.h"

/* Battery charge level is an integer ranging between 0 and 5.
 * https://git.kernel.org/pub/scm/network/ofono/ofono.git/tree/doc/handsfree-api.txt
 * To convert it to a percentage, multiply by 20.
 */
#define BATTERY_PERCENTAGE_SCALE 20

/**
 * @todo write docs
 */
class Phone : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name MEMBER m_name);
    Q_PROPERTY(QString status MEMBER currentCallStatus);
    Q_PROPERTY(QString number MEMBER currentCallNumber);
    Q_PROPERTY(QString carrier MEMBER m_carrier);
    Q_PROPERTY(int signal MEMBER m_signal);
    Q_PROPERTY(int battery MEMBER m_battery);
    Q_PROPERTY(int volume MEMBER m_volume);
    Q_PROPERTY(uint lastkey MEMBER m_key READ getLastKey WRITE setLastKey);

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
    void InitModem(OfonoModem &modemInfo);
    /** Clear current modem (used to change it) */
    void ClearCurrentModem();
    /** Update current modem when one is connected/disconnected. */
    void UpdateCurrentModem(void);
    /** Update phone data file. */
    void UpdatePhoneDataFile(void);

    void setLastKey(uint keyValue);
    uint getLastKey(void);

  signals:
    void refresh_phone_info(void);
    void update_call_status(void);
    void key_pressed(uint key_input);
    void open_bluetooth_manager(void);
    void save_phone_file(const QString &content);

  public slots:
    void OnCall(const QString &phoneNumber);
    void OnHangup();
    void OnOpenBluetoothManager();

  private slots:
    void OnCallStarted(const QDBusObjectPath &path, const QVariantMap &properties);
    void OnCallClosed(const QDBusObjectPath &path);
    void OnCallPropertyChanged(const QString &propertyName, const QDBusVariant &propertyValue);
    void OnNetworkInfoChanged(const QString &propertyName, const QDBusVariant &propertyValue);
    void OnHandsfreePropertyChanged(const QString &propertyName, const QDBusVariant &propertyValue);
    void OnVolumePropertyChanged(const QString &propertyName, const QDBusVariant &propertyValue);
    void OnModemPropertyChanged(const QString &propertyName, const QDBusVariant &propertyValue);
    void OnModemAdded(const QDBusObjectPath &path, const QVariantMap &properties);
    void OnModemRemoved(const QDBusObjectPath &path);

  private:
    // Ofono general interfaces
    OrgOfonoManagerInterface *ofonoInterface;
    OrgOfonoVoiceCallManagerInterface *ofonoVoiceCallManager;
    OrgOfonoNetworkRegistrationInterface *ofonoNetworkInfo;
    OrgOfonoHandsfreeInterface *ofonoHandsfreeInfo;
    OrgOfonoCallVolumeInterface *ofonoVolume;

    // Modem list objects
    ModemList modems;
    QList<OrgOfonoModemInterface *> ofonoModemInterfaces;
    int currentModemIndex;

    // Voice call objects
    OrgOfonoVoiceCallInterface *ofonoVoiceCall;
    QDBusObjectPath currentCallPath;

    // Exported properties
    QString currentCallNumber;
    QString currentCallStatus;
    QString m_name;
    QString m_carrier;
    int m_signal;
    int m_battery;
    int m_volume;
    uint m_key;
};

#endif // PHONE_H
