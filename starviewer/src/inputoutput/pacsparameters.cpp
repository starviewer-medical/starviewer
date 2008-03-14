
#include "pacsparameters.h"

namespace udg{

PacsParameters::PacsParameters( QString adr , QString port , QString aet , QString aec )
{
    m_Adr = adr;
    m_Port = port;
    m_aeTitle = aet;
    m_aeCalled = aec;
    m_TimeOut = 15000; //establim que per defecte el timeout és de 15000 ms
    m_PacsID = 0;
}

PacsParameters::PacsParameters()
{
    m_PacsID = 0;
}

void PacsParameters::setPacsAdr( QString adr )
{
     m_Adr = adr;
}

void PacsParameters::setPacsPort( QString port )
{
     m_Port = port;
}

void PacsParameters::setAELocal( QString aet )
{
     m_aeTitle = aet;
}

void PacsParameters::setAEPacs( QString aec )
{
     m_aeCalled = aec;
}

void PacsParameters::setLocalPort( QString port )
{
    m_LocalPort = port;
}

void PacsParameters::setInstitution( QString inst )
{
    m_Inst = inst;
}

void PacsParameters::setLocation( QString local )
{
    m_Location = local;
}

void PacsParameters::setDescription( QString desc )
{
    m_Desc = desc;
}

void PacsParameters::setDefault( QString def )
{
    m_Default = def;
}

void PacsParameters::setTimeOut( int time )
{
    m_TimeOut = time * 1000; //convertim a ms
}

void PacsParameters::setPacsID( int id )
{
    m_PacsID = id;
}

/*****************************************************************************************/

QString PacsParameters::getPacsAdr()
{
    return m_Adr;
}

QString PacsParameters::getPacsPort()
{
    return m_Port;
}

QString PacsParameters::getAELocal()
{
    return m_aeTitle;
}

QString PacsParameters::getAEPacs()
{
    return m_aeCalled;
}

QString PacsParameters::getLocalPort()
{
    return m_LocalPort;
}

QString PacsParameters::getInstitution()
{
    return m_Inst;
}

QString PacsParameters::getLocation()
{
    return m_Location;
}

QString PacsParameters::getDescription()
{
    return m_Desc;
}

QString PacsParameters::getDefault()
{
    return m_Default;
}

int PacsParameters::getTimeOut()
{
    return m_TimeOut;
}

int PacsParameters::getPacsID()
{
    return m_PacsID;
}

bool PacsParameters::operator ==(const PacsParameters &parameters)
{
    if(    m_aeCalled == parameters.m_aeCalled
        && m_aeTitle == parameters.m_aeTitle
        && m_Port == parameters.m_Port
        && m_Adr == parameters.m_Adr
        && m_LocalPort == parameters.m_LocalPort
        && m_Desc == parameters.m_Desc
        && m_Inst == parameters.m_Inst
        && m_Location == parameters.m_Location
        && m_Default == parameters.m_Default
        && m_Location == parameters.m_Location
        && m_PacsID == parameters.m_PacsID
        && m_TimeOut == parameters.m_TimeOut
    )
        return true;
    else
        return false;
}

}

