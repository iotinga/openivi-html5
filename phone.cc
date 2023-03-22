// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2023 <copyright holder> <email>
// SPDX-License-Identifier: GPL-2.0-or-later

#include "phone.h"

Phone::Phone(QObject *parent) : QObject(parent)
{
	m_name = "Disconnected";
	ofonoInterface = new OrgOfonoManagerInterface("org.ofono", "/", QDBusConnection::systemBus());
	ofonoVoiceCallManager = NULL;
	ofonoNetworkInfo = NULL;
	ofonoHandsfreeInfo = NULL;
	ofonoVoiceCall = NULL;
	ofonoVolume = NULL;

	currentCallStatus = "none";

	qDBusRegisterMetaType<ModemList>();
	qDBusRegisterMetaType<OfonoModem>();
	qDBusRegisterMetaType<CallList>();
	qDBusRegisterMetaType<OfonoCall>();

	InitOfono();

}

Phone::~Phone()
{
	if (ofonoVoiceCall != NULL) {
		delete ofonoVoiceCall;
		ofonoVoiceCall = NULL;
	}
	if (ofonoNetworkInfo != NULL) {
		delete ofonoNetworkInfo;
		ofonoNetworkInfo = NULL;
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
	delete ofonoInterface;
}

void Phone::InitOfono()
{
	int i = 0;
	QString modemName = "Unknown";
	bool online = false;
	if (ofonoInterface != NULL) {
		qDebug() << "Ofono interface initialized";
		ModemList modems = ofonoInterface->GetModems();
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
			qDebug() << "Modem [" << i << "] : " << modem.objectPath.path();
			qDebug() << "Modem name: " << modemName;
			qDebug() << "Online: " << online;
			// if (i == 0) {
			if (online) {
				m_name = modemName;
				if (online) {
					InitModem(modem);
					break;
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
					QString networkName = netNameIter.value().toString();
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
					m_battery = batteryIter.value().toInt();
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
	}
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
	}
}

void Phone::OnCallPropertyChanged(const QString& propertyName, const QDBusVariant& propertyValue)
{
	qDebug() << "Call property " << propertyName << " changed to " << propertyValue.variant();
	if (propertyName == "State") {
		currentCallStatus = propertyValue.variant().toString();
	}
}

void Phone::OnNetworkInfoChanged(const QString& propertyName, const QDBusVariant& propertyValue)
{
	qDebug() << "Network property " << propertyName << " changed to " << propertyValue.variant();
	if (propertyName == "Strength") {
		m_signal = propertyValue.variant().toInt();
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
