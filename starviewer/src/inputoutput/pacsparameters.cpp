
#include "pacsparameters.h"

namespace udg{

PacsParameters::PacsParameters(const QString &address, const QString &port, const QString &myAEtitle, const QString &remoteServerAEtitle)
{
    m_pacsAddress = address;
    m_pacsPort = port;
    m_myAETitle = myAEtitle;
    m_pacsAETitle = remoteServerAEtitle;
    m_connectionTimeOutInMilliseconds = 15000; //establim que per defecte el timeout és de 15000 ms
    m_pacsID = -1;
}

PacsParameters::PacsParameters()
{
    m_pacsID = -1;
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

void PacsParameters::setAELocal(const QString &AETitle)
{
    m_myAETitle = AETitle;
}

QString PacsParameters::getAELocal() const
{
    return m_myAETitle;
}

void PacsParameters::setAEPacs(const QString &remoteServerAETitle)
{
    m_pacsAETitle = remoteServerAETitle;
}

QString PacsParameters::getAEPacs() const
{
    return m_pacsAETitle;
}

void PacsParameters::setLocalPort(const QString &port)
{
    m_localPort = port;
}

QString PacsParameters::getLocalPort() const
{
    return m_localPort;
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

void PacsParameters::setDefault(const QString &isDefault)
{
    m_pacsIsDefault = isDefault;
}

QString PacsParameters::getDefault() const
{
    return m_pacsIsDefault;
}

void PacsParameters::setTimeOut(int timeoutInSeconds)
{
    m_connectionTimeOutInMilliseconds = timeoutInSeconds * 1000; //convertim a ms
}

int PacsParameters::getTimeOut() const
{
    return m_connectionTimeOutInMilliseconds;
}

void PacsParameters::setPacsID(int ID)
{
    m_pacsID = ID;
}

int PacsParameters::getPacsID() const
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
        && m_pacsIsDefault == parameters.m_pacsIsDefault
        && m_pacsLocation == parameters.m_pacsLocation
        && m_pacsID == parameters.m_pacsID
        && m_connectionTimeOutInMilliseconds == parameters.m_connectionTimeOutInMilliseconds;
}

}

