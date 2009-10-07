#include "pacsdevice.h"

#include "inputoutputsettings.h"
#include <QStringList>

namespace udg{

PacsDevice::PacsDevice()
{
}

void PacsDevice::setAddress(const QString &address)
{
    m_address = address;
}

QString PacsDevice::getAddress() const
{
    return m_address;
}

void PacsDevice::setPort(const QString &port)
{
    m_port = port;
}

QString PacsDevice::getPort() const
{
    return m_port;
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
    if( isDefault ) // afegir
    {
        if( !pacsList.contains( keyName ) ) // si no està marcat ja
        {
            Settings settings;
            QString value = settings.getValue( InputOutputSettings::DefaultPACSListToQuery ).toString();
            value += keyName + "//";
            settings.setValue( InputOutputSettings::DefaultPACSListToQuery, value );
        }
    }
    else // eliminar
    {
        Settings settings;
        QString value = settings.getValue( InputOutputSettings::DefaultPACSListToQuery ).toString();
        value.remove( keyName + "//" );
        settings.setValue( InputOutputSettings::DefaultPACSListToQuery, value );
    }
}

bool PacsDevice::isDefault() const
{
    QStringList pacsList = getDefaultPACSKeyNamesList();
    if( pacsList.contains( getKeyName() ) )
        return true;
    else
        return false;
}

void PacsDevice::setID(const QString &id)
{
    m_id = id;
}

QString PacsDevice::getID() const
{
    return m_id;
}

bool PacsDevice::isEmpty() const
{
    if( m_AETitle.isEmpty() &&
        m_port.isEmpty() &&
        m_address.isEmpty() &&
        m_description.isEmpty() &&
        m_institution.isEmpty() &&
        m_location.isEmpty() &&
        m_id.isEmpty() )
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool PacsDevice::operator ==(const PacsDevice &device)
{
    return m_AETitle == device.m_AETitle
        && m_port == device.m_port
        && m_address == device.m_address
        && m_description == device.m_description
        && m_institution == device.m_institution
        && m_location == device.m_location
        && m_id == device.m_id;
}

QString PacsDevice::getLocalAETitle()
{
    Settings settings;
    return settings.getValue( InputOutputSettings::LocalAETitle ).toString();
}

int PacsDevice::getMaximumConnections()
{
    Settings settings;
    return settings.getValue( InputOutputSettings::MaximumPACSConnections ).toInt();
}

int PacsDevice::getConnectionTimeout()
{
    Settings settings;
    return settings.getValue( InputOutputSettings::PacsConnectionTimeout ).toInt();
}

int PacsDevice::getQueryRetrievePort()
{
    Settings settings;
    return settings.getValue( InputOutputSettings::QueryRetrieveLocalPort ).toInt();
}

QString PacsDevice::getKeyName() const
{
    return m_AETitle + m_address;
}

QStringList PacsDevice::getDefaultPACSKeyNamesList() const
{
    Settings settings;
    QString value = settings.getValue( InputOutputSettings::DefaultPACSListToQuery ).toString();
    QStringList pacsList = value.split("//",QString::SkipEmptyParts);

    return pacsList;
}

}

