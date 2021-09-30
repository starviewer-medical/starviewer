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

#include "logging.h"
#include "coresettings.h"
#include <QStringList>

namespace udg {

PacsDevice::PacsDevice()
{
    m_queryRetrieveServicePort = -1;
    m_storeServicePort = -1;
}

void PacsDevice::setAddress(const QString &address)
{
    m_address = address;
}

QString PacsDevice::getAddress() const
{
    return m_address;
}

void PacsDevice::setQueryRetrieveServicePort(int queryRetrieveServicePort)
{
    m_queryRetrieveServicePort = queryRetrieveServicePort;
}

int PacsDevice::getQueryRetrieveServicePort() const
{
    return m_queryRetrieveServicePort;
}

void PacsDevice::setAETitle(const QString &AETitle)
{
    m_AETitle = AETitle;
}

QString PacsDevice::getAETitle() const
{
    return m_AETitle;
}

void PacsDevice::setInstitution(const QString &institution)
{
    m_institution = institution;
}

QString PacsDevice::getInstitution() const
{
    return m_institution;
}

void PacsDevice::setLocation(const QString &location)
{
    m_location = location;
}

QString PacsDevice::getLocation() const
{
    return m_location;
}

void PacsDevice::setDescription(const QString &description)
{
    m_description = description;
}

QString PacsDevice::getDescription() const
{
    return m_description;
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
            value += keyName + "//";
            settings.setValue(CoreSettings::DefaultPACSListToQuery, value);
        }
    }
    else
    {
        // Eliminar
        Settings settings;
        QString value = settings.getValue(CoreSettings::DefaultPACSListToQuery).toString();
        value.remove(keyName + "//");
        settings.setValue(CoreSettings::DefaultPACSListToQuery, value);
    }
}

bool PacsDevice::isDefault() const
{
    QStringList pacsList = getDefaultPACSKeyNamesList();
    if (pacsList.contains(getKeyName()))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void PacsDevice::setID(const QString &id)
{
    m_id = id;
}

QString PacsDevice::getID() const
{
    return m_id;
}

/// Assigna/Retorna si podem fer consultes/descarregues al PACS
void PacsDevice::setQueryRetrieveServiceEnabled(bool isQueryRetrieveServiceEnabled)
{
    m_isQueryRetrieveServiceEnabled = isQueryRetrieveServiceEnabled;
}

bool PacsDevice::isQueryRetrieveServiceEnabled() const
{
    return m_isQueryRetrieveServiceEnabled;
}

void PacsDevice::setStoreServiceEnabled(bool isStoreServiceEnabled)
{
    m_isStoreServiceEnabled = isStoreServiceEnabled;
}

bool PacsDevice::isStoreServiceEnabled() const
{
    return m_isStoreServiceEnabled;
}

void PacsDevice::setStoreServicePort(int storeServicePort)
{
    m_storeServicePort = storeServicePort;
}

int PacsDevice::getStoreServicePort() const
{
    return m_storeServicePort;
}

bool PacsDevice::isEmpty() const
{
    if (m_AETitle.isEmpty() &&
        m_address.isEmpty() &&
        m_description.isEmpty() &&
        m_institution.isEmpty() &&
        m_location.isEmpty() &&
        m_id.isEmpty())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PacsDevice::isSamePacsDevice(const PacsDevice &pacsDevice) const
{
    return m_AETitle == pacsDevice.getAETitle()
        && m_address == pacsDevice.getAddress()
        && m_queryRetrieveServicePort == pacsDevice.getQueryRetrieveServicePort();
}

bool PacsDevice::operator ==(const PacsDevice &device) const
{
    return m_AETitle == device.m_AETitle
        && m_address == device.m_address
        && m_description == device.m_description
        && m_institution == device.m_institution
        && m_location == device.m_location
        && m_id == device.m_id
        && m_isQueryRetrieveServiceEnabled == device.m_isQueryRetrieveServiceEnabled
        && m_queryRetrieveServicePort == device.m_queryRetrieveServicePort
        && m_isStoreServiceEnabled == device.m_isStoreServiceEnabled
        && m_storeServicePort == device.m_storeServicePort;
}

QString PacsDevice::getKeyName() const
{
    return m_AETitle + m_address + ":" + QString::number(m_queryRetrieveServicePort);
}

QStringList PacsDevice::getDefaultPACSKeyNamesList() const
{
    Settings settings;
    QString value = settings.getValue(CoreSettings::DefaultPACSListToQuery).toString();
    QStringList pacsList = value.split("//", QString::SkipEmptyParts);

    if (pacsList.isEmpty())
    {
        INFO_LOG("No default PACS.");
    }

    return pacsList;
}

}
