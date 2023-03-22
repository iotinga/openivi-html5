/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -p OfonoVoiceCallManager phone_hfp_annotated.xml org.ofono.VoiceCallManager
 *
 * qdbusxml2cpp is Copyright (C) 2020 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * This file may have been hand-edited. Look for HAND-EDIT comments
 * before re-generating it.
 */

#include "OfonoVoiceCallManager.h"

// Marshall the OfonoCall data into a D-Bus argument
QDBusArgument &operator<<(QDBusArgument &argument, const OfonoCall &call)
{
    argument.beginStructure();
    argument << call.objectPath << call.properties;
    argument.endStructure();
    return argument;
}

// Retrieve the OfonoCall data from the D-Bus argument
const QDBusArgument &operator>>(const QDBusArgument &argument, OfonoCall &call)
{
    argument.beginStructure();
    argument >> call.objectPath >> call.properties;
    argument.endStructure();
    return argument;
}

/*
 * Implementation of interface class OrgOfonoVoiceCallManagerInterface
 */

OrgOfonoVoiceCallManagerInterface::OrgOfonoVoiceCallManagerInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent)
    : QDBusAbstractInterface(service, path, staticInterfaceName(), connection, parent)
{
}

OrgOfonoVoiceCallManagerInterface::~OrgOfonoVoiceCallManagerInterface()
{
}

