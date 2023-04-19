// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "phone.h"

#define NO_ACTIVE_MODEM -1

Phone::Phone(QObject *parent) : QObject(parent)
{
	m_name = "Disconnected";
	currentCallNumber.clear();
	currentCallStatus = "none";
	m_carrier.clear();
	m_signal = 0;
	m_battery = 0;
	m_volume = 0;
	m_key = 0;
	ofonoInterface = new OrgOfonoManagerInterface("org.ofono", "/", QDBusConnection::systemBus());
	ofonoVoiceCallManager = NULL;
	ofonoNetworkInfo = NULL;
	ofonoHandsfreeInfo = NULL;
	ofonoVoiceCall = NULL;
	ofonoVolume = NULL;
	currentModemIndex = NO_ACTIVE_MODEM;

	qDBusRegisterMetaType<ModemList>();
	qDBusRegisterMetaType<OfonoModem>();
	qDBusRegisterMetaType<CallList>();
	qDBusRegisterMetaType<OfonoCall>();

	InitOfono();

}

Phone::~Phone()
{
	for (int mm = 0; mm < ofonoModemInterfaces.size(); mm++) {
		delete ofonoModemInterfaces[mm];
	}
	ofonoModemInterfaces.clear();
	if (ofonoNetworkInfo != NULL) {
		delete ofonoNetworkInfo;
		ofonoNetworkInfo = NULL;
	}

	ClearCurrentModem();

	delete ofonoInterface;
}

void Phone::ClearCurrentModem()
{
	if (ofonoVoiceCall != NULL) {
		delete ofonoVoiceCall;
		ofonoVoiceCall = NULL;
	}
	if (ofonoHandsfreeInfo != NULL) {
		delete ofonoHandsfreeInfo;
		ofonoHandsfreeInfo = NULL;
	}
	if (ofonoVolume != NULL) {
		delete ofonoVolume;
		ofonoVolume = NULL;
	}
	if (ofonoVoiceCallManager != NULL) {
		delete ofonoVoiceCallManager;
		ofonoVoiceCallManager = NULL;
	}
	currentModemIndex = NO_ACTIVE_MODEM;
}

void Phone::setLastKey(uint keyValue)
{
	m_key = keyValue;
	// key_pressed();
}

uint Phone::getLastKey()
{
	return m_key;
}

void Phone::InitOfono()
{
	int i = 0;
	QString modemName = "Unknown";
	bool online = false, onlineFound = false;
	OrgOfonoModemInterface* modemIface = NULL;
	if (ofonoInterface != NULL) {
		qDebug() << "Ofono interface initialized";

		connect(ofonoInterface, SIGNAL(ModemAdded(const QDBusObjectPath &, const QVariantMap &)), this, SLOT(OnModemAdded(const QDBusObjectPath &, const QVariantMap &)));
    connect(ofonoInterface, SIGNAL(ModemRemoved(const QDBusObjectPath &)), this, SLOT(OnModemRemoved(const QDBusObjectPath&)));

		// ModemList modems = ofonoInterface->GetModems();
		modems = ofonoInterface->GetModems();
		qDebug() << "Found " << modems.size() << " modem";
		for (i = 0; i < modems.size(); i++) {
			OfonoModem modem = modems.value(i);
			QMap<QString, QVariant>::const_iterator modemNameIter = modem.properties.find("Name");
			if (modemNameIter != modem.properties.end()) {
				modemName = modemNameIter.value().toString();
			}
			QMap<QString, QVariant>::const_iterator modemOnlineIter = modem.properties.find("Online");
			if (modemOnlineIter != modem.properties.end()) {
				online = modemOnlineIter.value().toBool();
			}
			// Init a modem interface to catch connect/disconnect events
			modemIface = new OrgOfonoModemInterface("org.ofono", modem.objectPath.path(), QDBusConnection::systemBus());
			if (modemIface != NULL) {
				connect(modemIface, SIGNAL(PropertyChanged(QString, QDBusVariant)), this, SLOT(OnModemPropertyChanged(QString, QDBusVariant)));
				ofonoModemInterfaces.append(modemIface);
			}

			qDebug() << "Modem [" << i << "] : " << modem.objectPath.path();
			qDebug() << "Modem name: " << modemName;
			qDebug() << "Online: " << online;
			// if (i == 0) {
			if (online) {
				m_name = modemName;
				if (!onlineFound) {
					InitModem(modem);
					currentModemIndex = i;
					// break;
					onlineFound = true;
				}
			}
		}
	}
}

void Phone::InitModem(OfonoModem& modemInfo)
{
	ofonoVoiceCallManager = new OrgOfonoVoiceCallManagerInterface("org.ofono", modemInfo.objectPath.path(), QDBusConnection::systemBus());
	if (ofonoVoiceCallManager) {
		connect(ofonoVoiceCallManager, SIGNAL(CallAdded(QDBusObjectPath, QVariantMap)), this, SLOT(OnCallStarted(QDBusObjectPath, QVariantMap)));
		connect(ofonoVoiceCallManager, SIGNAL(CallRemoved(QDBusObjectPath)), this, SLOT(OnCallClosed(QDBusObjectPath)));
		// Init Network Info
		ofonoNetworkInfo = new OrgOfonoNetworkRegistrationInterface("org.ofono", modemInfo.objectPath.path(), QDBusConnection::systemBus());
		if (ofonoNetworkInfo != NULL) {
			QVariantMap netInfo = ofonoNetworkInfo->GetProperties();
			if (netInfo.size() > 0) {
				QMap<QString, QVariant>::const_iterator netNameIter = netInfo.find("Name");
				if (netNameIter != netInfo.end()) {
					m_carrier = netNameIter.value().toString();
					// ui->operatorLabel->setText(networkName);
				}
				QMap<QString, QVariant>::const_iterator netSignalIter = netInfo.find("Strength");
				if (netSignalIter != netInfo.end()) {
					m_signal = netSignalIter.value().toInt();
					// ui->signalStrength->setValue(networkStrength);
				}
			}
			connect(ofonoNetworkInfo, SIGNAL(PropertyChanged(QString, QDBusVariant)), this, SLOT(OnNetworkInfoChanged(QString, QDBusVariant)));
		}
		// Init handsfree interface to read battery level
		ofonoHandsfreeInfo = new OrgOfonoHandsfreeInterface("org.ofono", modemInfo.objectPath.path(), QDBusConnection::systemBus());
		if (ofonoHandsfreeInfo != NULL) {
			QVariantMap hfInfo = ofonoHandsfreeInfo->GetProperties();
			if (hfInfo.size() > 0) {
				QMap<QString, QVariant>::const_iterator batteryIter = hfInfo.find("BatteryChargeLevel");
				if (batteryIter != hfInfo.end()) {
					m_battery = batteryIter.value().toInt()*BATTERY_PERCENTAGE_SCALE;
					// ui->batteryLevel->setValue(batteryValue*20);
				}
			}
			connect(ofonoHandsfreeInfo, SIGNAL(PropertyChanged(QString, QDBusVariant)), this, SLOT(OnHandsfreePropertyChanged(QString, QDBusVariant)));
		}
		// Init volume controls
		ofonoVolume = new OrgOfonoCallVolumeInterface("org.ofono", modemInfo.objectPath.path(), QDBusConnection::systemBus());
		if (ofonoVolume != NULL) {
			QVariantMap volumeInfo = ofonoVolume->GetProperties();
			if (volumeInfo.size() > 0) {
				QMap<QString, QVariant>::const_iterator speakerIter = volumeInfo.find("SpeakerVolume");
				if (speakerIter != volumeInfo.end()) {
					m_volume = speakerIter.value().toInt();
					// ui->volumeSlider->setValue(speakerVolumeLevel);
				}
			}
			connect(ofonoVolume, SIGNAL(PropertyChanged(QString, QDBusVariant)), this, SLOT(OnVolumePropertyChanged(QString, QDBusVariant)));
		}
		/* Once everything is set up, emit refresh event to set up
		 * phone status to HTML5 interface. */
		refresh_phone_info();
	}
}

void Phone::OnModemAdded(const QDBusObjectPath &path, const QVariantMap &properties)
{
	OrgOfonoModemInterface* modemIface = NULL;
	qDebug() << "Added ofono modem at " << path.path() << " with " << properties.size() << " properties";

	// Init a modem interface to catch connect/disconnect events
	modemIface = new OrgOfonoModemInterface("org.ofono", path.path(), QDBusConnection::systemBus());
	if (modemIface != NULL) {
		connect(modemIface, SIGNAL(PropertyChanged(QString, QDBusVariant)), this, SLOT(OnModemPropertyChanged(QString, QDBusVariant)));
		ofonoModemInterfaces.append(modemIface);
	}

	UpdateCurrentModem();
	UpdatePhoneDataFile();
}

void Phone::OnModemRemoved(const QDBusObjectPath &path)
{
	qDebug() << "Removed ofono modem at " << path.path();

	OrgOfonoModemInterface* modemIface = NULL;

	for (int mdm = 0; mdm < ofonoModemInterfaces.size(); mdm++) {
		modemIface = ofonoModemInterfaces.at(mdm);
		if (modemIface) {
			if (modemIface->path() == path.path()) {
				disconnect(modemIface, SIGNAL(PropertyChanged(QString, QDBusVariant)), this, SLOT(OnModemPropertyChanged(QString, QDBusVariant)));
				ofonoModemInterfaces.removeAt(mdm);
				break;
			}
		}
	}

	UpdateCurrentModem();
	UpdatePhoneDataFile();
}

void Phone::OnCall(const QString& phoneNumber)
{
	if (ofonoVoiceCallManager != NULL) {
		if (ofonoVoiceCall != NULL) {
			// Call was initiated by remote caller, answer
			ofonoVoiceCall->Answer();
			qDebug() << "Answering to " << currentCallNumber;
		} else {
			ofonoVoiceCallManager->Dial(phoneNumber, "");
			qDebug() << "Calling " << phoneNumber;
		}
	}
}

void Phone::OnHangup()
{
	if (ofonoVoiceCallManager != NULL) {
		ofonoVoiceCallManager->HangupAll();
		qDebug() << "Hanging up...";
	}
}

void Phone::OnOpenBluetoothManager()
{
	open_bluetooth_manager();
}


void Phone::OnCallStarted(const QDBusObjectPath& path, const QVariantMap& properties)
{
	currentCallPath.setPath(path.path());
	QMap<QString, QVariant>::const_iterator callNumberIter = properties.find("LineIdentification");
	QMap<QString, QVariant>::const_iterator callStatusIter = properties.find("State");
	if ((callNumberIter != properties.end()) && (callStatusIter != properties.end())) {
		currentCallNumber = callNumberIter.value().toString();
		currentCallStatus = callStatusIter.value().toString();
		// Initialize call instance
		ofonoVoiceCall = new OrgOfonoVoiceCallInterface("org.ofono", currentCallPath.path(), QDBusConnection::systemBus());
		if (ofonoVoiceCall != NULL) {
			connect(ofonoVoiceCall, SIGNAL(PropertyChanged(QString, QDBusVariant)), this, SLOT(OnCallPropertyChanged(QString, QDBusVariant)));
		}
		if (currentCallNumber == "incoming") {
			qDebug() << "Incoming call from " << currentCallNumber;
		} else if (currentCallNumber == "alerting") {
			qDebug() << "Waiting for response...";
		} else {
		}
		update_call_status();
	} else {
		qDebug() << "New call: error reading properties!";
	}
}

void Phone::OnCallClosed(const QDBusObjectPath& path)
{
	if (path.path() == currentCallPath.path()) {
		currentCallPath.setPath("/");
		currentCallNumber.clear();
		currentCallStatus = "none";
		if (ofonoVoiceCall != NULL) {
			delete ofonoVoiceCall;
			ofonoVoiceCall = NULL;
		}
		qDebug() << "Current call terminated";
		update_call_status();
	}
}

void Phone::OnCallPropertyChanged(const QString& propertyName, const QDBusVariant& propertyValue)
{
	qDebug() << "Call property " << propertyName << " changed to " << propertyValue.variant();
	if (propertyName == "State") {
		currentCallStatus = propertyValue.variant().toString();
		update_call_status();
	}
}

void Phone::OnNetworkInfoChanged(const QString& propertyName, const QDBusVariant& propertyValue)
{
	qDebug() << "Network property " << propertyName << " changed to " << propertyValue.variant();
	if (propertyName == "Strength") {
		m_signal = propertyValue.variant().toInt();
		refresh_phone_info();
	}
}

void Phone::OnHandsfreePropertyChanged(const QString& propertyName, const QDBusVariant& propertyValue)
{
	qDebug() << "Handsfree property " << propertyName << " changed to " << propertyValue.variant();
	if (propertyName == "BatteryChargeLevel") {
		m_battery = propertyValue.variant().toInt()*20;
	}
}

void Phone::OnVolumePropertyChanged(const QString& propertyName, const QDBusVariant& propertyValue)
{
	qDebug() << "Volume property " << propertyName << " changed to " << propertyValue.variant();
	if (propertyName == "SpeakerVolume") {
		m_volume = propertyValue.variant().toInt();
	}
}

void Phone::OnModemPropertyChanged(const QString& propertyName, const QDBusVariant& propertyValue)
{
	qDebug() << "Modem property " << propertyName << " changed to " << propertyValue.variant();
	if (propertyName == "Online") {
		QThread::msleep(500);
		UpdateCurrentModem();
	}
}

void Phone::UpdateCurrentModem()
{
	/* As the PropertyChanged signal does not contain the reference
	 * to the emitter, it is easier to scan the modem list and set
	 * new current modem. It should not be a performance issue, as
	 * normally a device has only few paired phones. */
	/* For this demo application, only one phone/modem is considered
	 * connected at once, even though more than one device may be
	 * available. So, when multiple ofono modems are online, the
	 * selection logic is as follows:
	 * - If previously connected modem is still connected, do
	 *   nothing.
	 * - Otherwise, select the fist onlin modem in the modem list
	 *   (which should be in chronological order of pairing). */
	int mm = -1;
	bool isOnline = false;

	// Re-read modem info from ofono interface
	modems.clear();
	modems = ofonoInterface->GetModems();

	if (currentModemIndex != NO_ACTIVE_MODEM) {
		OfonoModem currentModem = modems.value(currentModemIndex);
		QMap<QString, QVariant>::const_iterator modemOnlineIter = currentModem.properties.find("Online");
		if (modemOnlineIter != currentModem.properties.end()) {
			bool currentOnline = modemOnlineIter.value().toBool();
			if (currentOnline) {
				return;
			}
		}
	}
	// Previous current is not online, or there were no online modems
	for (mm = 0; mm < modems.size(); mm++) {
		OfonoModem modem = modems.value(mm);
		QMap<QString, QVariant>::const_iterator modemIter = modem.properties.find("Online");
		if (modemIter != modem.properties.end()) {
			isOnline = modemIter.value().toBool();
			if (isOnline) {
				// One modem online found: use it and exit loop
				ClearCurrentModem();
				QMap<QString, QVariant>::const_iterator modemNameIter = modem.properties.find("Name");
				if (modemNameIter != modem.properties.end()) {
					m_name = modemNameIter.value().toString();
				}
				InitModem(modem);
				currentModemIndex = mm;
				break;
			}
		}
	}
	if (mm >= modems.size()) {
		// No online modem
		ClearCurrentModem();
		currentModemIndex = NO_ACTIVE_MODEM;
		m_name = "Disconnected";
		m_carrier.clear();
		m_signal = 0;
		m_battery = 0;
		m_volume = 0;
		refresh_phone_info();
	}
}

void Phone::UpdatePhoneDataFile()
{
	QString fileContent("var phoneLib =[");
	QString modemString("{name:\"%1\",mac:\"%2\",file:\"phone1.js\"},");
	QString modemName;
	QString modemMac;
	QString modemPath;
	int mm = -1, devIdx = -1;

		// Previous current is not online, or there were no online modems
	for (mm = 0; mm < modems.size(); mm++) {
		OfonoModem modem = modems.value(mm);
		modemPath = modem.objectPath.path();
		devIdx = modemPath.indexOf("dev_");
		if (devIdx != -1) {
			modemMac = modemPath.mid(devIdx + 4);
			modemMac.replace('_', ':');
		}
		QMap<QString, QVariant>::const_iterator modemNameIter = modem.properties.find("Name");
		if (modemNameIter != modem.properties.end()) {
			modemName = modemNameIter.value().toString();
		}
		fileContent.append(modemString.arg(modemName).arg(modemMac));
	}

	fileContent.append("]");

	save_phone_file(fileContent);
}
