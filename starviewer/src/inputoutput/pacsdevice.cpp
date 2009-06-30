#include "pacsdevice.h"

#include "inputoutputsettings.h"
#include <QHostInfo> // pel getLocalAETitle()

namespace udg{

PacsDevice::PacsDevice(const QString &address, const QString &port, const QString &myAEtitle, const QString &remoteServerAEtitle)
{
    m_pacsAddress = address;
    m_pacsPort = port;
    m_myAETitle = myAEtitle;
    m_pacsAETitle = remoteServerAEtitle;
    m_connectionTimeOut = 15; //establim que per defecte el timeout és de 15 ms
    m_pacsID = "";
}

PacsDevice::PacsDevice()
{
    m_pacsID = "";
}

void PacsDevice::setPacsAddress(const QString &address)
{
    m_pacsAddress = address;
}

QString PacsDevice::getPacsAddress() const
{
    return m_pacsAddress;
}

void PacsDevice::setPacsPort(const QString &port)
{
    m_pacsPort = port;
}

QString PacsDevice::getPacsPort() const
{
    return m_pacsPort;
}

void PacsDevice::setAEPacs(const QString &remoteServerAETitle)
{
    m_pacsAETitle = remoteServerAETitle;
}

QString PacsDevice::getAEPacs() const
{
    return m_pacsAETitle;
}

void PacsDevice::setInstitution(const QString &institution)
{
    m_pacsInstitution = institution;
}

QString PacsDevice::getInstitution() const
{
    return m_pacsInstitution;
}

void PacsDevice::setLocation(const QString &location)
{
    m_pacsLocation = location;
}

QString PacsDevice::getLocation() const
{
    return m_pacsLocation;
}

void PacsDevice::setDescription(const QString &description)
{
    m_pacsDescription = description;
}

QString PacsDevice::getDescription() const
{
    return m_pacsDescription;
}

void PacsDevice::setDefault(bool isDefault)
{
    m_isDefaultPACS = isDefault;
}

bool PacsDevice::isDefault() const
{
    return m_isDefaultPACS;
}

void PacsDevice::setPacsID(QString ID)
{
    m_pacsID = ID;
}

QString PacsDevice::getPacsID() const
{
    return m_pacsID;
}

bool PacsDevice::operator ==(const PacsDevice &parameters)
{
    return m_pacsAETitle == parameters.m_pacsAETitle
        && m_myAETitle == parameters.m_myAETitle
        && m_pacsPort == parameters.m_pacsPort
        && m_pacsAddress == parameters.m_pacsAddress
        && m_localPort == parameters.m_localPort
        && m_pacsDescription == parameters.m_pacsDescription
        && m_pacsInstitution == parameters.m_pacsInstitution
        && m_pacsLocation == parameters.m_pacsLocation
        && isDefault() == parameters.isDefault()
        && m_pacsLocation == parameters.m_pacsLocation
        && m_pacsID == parameters.m_pacsID
        && m_connectionTimeOut == parameters.m_connectionTimeOut;
}

QString PacsDevice::getLocalAETitle()
{
    Settings settings;
    return settings.getValue( InputOutputSettings::localAETitleKey ).toString();
}

int PacsDevice::getMaximumConnections()
{
    Settings settings;
    return settings.getValue( InputOutputSettings::maximumPACSConnectionsKey ).toInt();
}

int PacsDevice::getConnectionTimeout()
{
    Settings settings;
    return settings.getValue( InputOutputSettings::pacsConnectionTimeoutKey ).toInt();
}

int PacsDevice::getQueryRetrievePort()
{
    Settings settings;
    return settings.getValue( InputOutputSettings::queryRetrieveLocalPortKey ).toInt();
}

}

