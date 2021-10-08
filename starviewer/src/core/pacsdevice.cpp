/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "pacsdevice.h"

#include "coresettings.h"
#include "logging.h"

namespace udg {

namespace {

const QString DefaultPacsListSeparator("\\\\");

}

PacsDevice::PacsDevice()
    : m_type(Type::Dimse), m_isQueryRetrieveServiceEnabled(false), m_queryRetrieveServicePort(-1), m_isStoreServiceEnabled(false), m_storeServicePort(-1)
{
}

const QString& PacsDevice::getID() const
{
    return m_id;
}

void PacsDevice::setID(QString id)
{
    m_id = std::move(id);
}

PacsDevice::Type PacsDevice::getType() const
{
    return m_type;
}

void PacsDevice::setType(Type type)
{
    m_type = type;
}

const QString& PacsDevice::getAETitle() const
{
    return m_AETitle;
}

void PacsDevice::setAETitle(QString aeTitle)
{
    m_AETitle = std::move(aeTitle);
}

const QString& PacsDevice::getAddress() const
{
    return m_address;
}

void PacsDevice::setAddress(QString address)
{
    m_address = std::move(address);
}

bool PacsDevice::isQueryRetrieveServiceEnabled() const
{
    return m_isQueryRetrieveServiceEnabled;
}

void PacsDevice::setQueryRetrieveServiceEnabled(bool enabled)
{
    m_isQueryRetrieveServiceEnabled = enabled;
}

int PacsDevice::getQueryRetrieveServicePort() const
{
    return m_queryRetrieveServicePort;
}

void PacsDevice::setQueryRetrieveServicePort(int port)
{
    m_queryRetrieveServicePort = port;
}

bool PacsDevice::isStoreServiceEnabled() const
{
    return m_isStoreServiceEnabled;
}

void PacsDevice::setStoreServiceEnabled(bool enabled)
{
    m_isStoreServiceEnabled = enabled;
}

int PacsDevice::getStoreServicePort() const
{
    return m_storeServicePort;
}

void PacsDevice::setStoreServicePort(int port)
{
    m_storeServicePort = port;
}

const QUrl& PacsDevice::getBaseUri() const
{
    return m_baseUri;
}

void PacsDevice::setBaseUri(QUrl baseUri)
{
    m_baseUri = std::move(baseUri);
}

const QString& PacsDevice::getInstitution() const
{
    return m_institution;
}

void PacsDevice::setInstitution(QString institution)
{
    m_institution = std::move(institution);
}

const QString& PacsDevice::getLocation() const
{
    return m_location;
}

void PacsDevice::setLocation(QString location)
{
    m_location = std::move(location);
}

const QString& PacsDevice::getDescription() const
{
    return m_description;
}

void PacsDevice::setDescription(QString description)
{
    m_description = std::move(description);
}

bool PacsDevice::isDefault() const
{
    QStringList pacsList = getDefaultPACSKeyNamesList();
    return pacsList.contains(getKeyName());
}

void PacsDevice::setDefault(bool isDefault)
{
    QStringList pacsList = getDefaultPACSKeyNamesList();
    QString keyName = getKeyName();
    // Afegir
    if (isDefault)
    {
        // Si no està marcat ja
        if (!pacsList.contains(keyName))
        {
            Settings settings;
            QString value = settings.getValue(CoreSettings::DefaultPACSListToQuery).toString();
            value += keyName + DefaultPacsListSeparator;
            settings.setValue(CoreSettings::DefaultPACSListToQuery, value);
        }
    }
    else
    {
        // Eliminar
        Settings settings;
        QString value = settings.getValue(CoreSettings::DefaultPACSListToQuery).toString();
        value.remove(keyName + DefaultPacsListSeparator);
        settings.setValue(CoreSettings::DefaultPACSListToQuery, value);
    }
}

bool PacsDevice::isEmpty() const
{
    return *this == PacsDevice();
}

bool PacsDevice::isSamePacsDevice(const PacsDevice &pacsDevice) const
{
    return (m_type == Type::Dimse
            && m_AETitle == pacsDevice.getAETitle()
            && m_address == pacsDevice.getAddress()
            && m_queryRetrieveServicePort == pacsDevice.getQueryRetrieveServicePort())
        || (m_type == Type::Wado
            && m_baseUri == pacsDevice.getBaseUri());
}

bool PacsDevice::operator==(const PacsDevice &pacsDevice) const
{
    return m_id == pacsDevice.m_id
        && m_type == pacsDevice.m_type
        && m_AETitle == pacsDevice.m_AETitle
        && m_address == pacsDevice.m_address
        && m_isQueryRetrieveServiceEnabled == pacsDevice.m_isQueryRetrieveServiceEnabled
        && m_queryRetrieveServicePort == pacsDevice.m_queryRetrieveServicePort
        && m_isStoreServiceEnabled == pacsDevice.m_isStoreServiceEnabled
        && m_storeServicePort == pacsDevice.m_storeServicePort
        && m_baseUri == pacsDevice.m_baseUri
        && m_location == pacsDevice.m_location
        && m_institution == pacsDevice.m_institution
        && m_description == pacsDevice.m_description;
}

QString PacsDevice::getKeyName() const
{
    if (m_type == Type::Dimse)
    {
        return m_AETitle + m_address + ":" + QString::number(m_queryRetrieveServicePort);
    }
    else // m_type == Type::Wado
    {
        return m_baseUri.toString();
    }
}

QStringList PacsDevice::getDefaultPACSKeyNamesList() const
{
    const static QString OldSeparator("//");

    Settings settings;
    QString listString = settings.getValue(CoreSettings::DefaultPACSListToQuery).toString();

    // Migrate from old format to new if needed
    if (listString.endsWith(OldSeparator))
    {
        listString = listString.split(OldSeparator).join(DefaultPacsListSeparator);
        settings.setValue(CoreSettings::DefaultPACSListToQuery, listString);
    }

    QStringList pacsList = listString.split(DefaultPacsListSeparator, QString::SkipEmptyParts);

    if (pacsList.isEmpty())
    {
        INFO_LOG("No default PACS.");
    }

    return pacsList;
}

}
