
#include "pacsparameters.h"

namespace udg{

PacsParameters::PacsParameters(const QString &address, const QString &port, const QString &myAEtitle, const QString &remoteServerAEtitle)
{
    m_pacsAddress = address;
    m_pacsPort = port;
    m_myAETitle = myAEtitle;
    m_pacsAETitle = remoteServerAEtitle;
    m_connectionTimeOut = 15; //establim que per defecte el timeout és de 15 ms
    m_pacsID = "";
}

PacsParameters::PacsParameters()
{
    m_pacsID = "";
}

void PacsParameters::setPacsAddress(const QString &address)
{
    m_pacsAddress = address;
}

QString PacsParameters::getPacsAddress() const
{
    return m_pacsAddress;
}

void PacsParameters::setPacsPort(const QString &port)
{
    m_pacsPort = port;
}

QString PacsParameters::getPacsPort() const
{
    return m_pacsPort;
}

void PacsParameters::setAEPacs(const QString &remoteServerAETitle)
{
    m_pacsAETitle = remoteServerAETitle;
}

QString PacsParameters::getAEPacs() const
{
    return m_pacsAETitle;
}

void PacsParameters::setInstitution(const QString &institution)
{
    m_pacsInstitution = institution;
}

QString PacsParameters::getInstitution() const
{
    return m_pacsInstitution;
}

void PacsParameters::setLocation(const QString &location)
{
    m_pacsLocation = location;
}

QString PacsParameters::getLocation() const
{
    return m_pacsLocation;
}

void PacsParameters::setDescription(const QString &description)
{
    m_pacsDescription = description;
}

QString PacsParameters::getDescription() const
{
    return m_pacsDescription;
}

void PacsParameters::setDefault(bool isDefault)
{
    m_isDefaultPACS = isDefault;
}

bool PacsParameters::isDefault() const
{
    return m_isDefaultPACS;
}

void PacsParameters::setPacsID(QString ID)
{
    m_pacsID = ID;
}

QString PacsParameters::getPacsID() const
{
    return m_pacsID;
}

bool PacsParameters::operator ==(const PacsParameters &parameters)
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

}

