#include "pacsparameters.h"
#include "const.h"
#include "study.h"
#include <string.h>

namespace udg{

PacsParameters::PacsParameters( std::string adr , std::string port , std::string aet , std::string aec )
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

void PacsParameters::setPacsAdr( std::string adr )
{     
     m_Adr = adr;
}  

void PacsParameters::setPacsPort( std::string port )
{     
     m_Port = port;
}  

void PacsParameters::setAELocal( std::string aet )
{     
     m_aeTitle = aet;
}  

void PacsParameters::setAEPacs( std::string aec )
{     
     m_aeCalled = aec;
}  

void PacsParameters::setLocalPort( std::string port )
{
    m_LocalPort = port;
}

void PacsParameters::setInstitution( std::string inst )
{
    m_Inst = inst;
}

void PacsParameters::setLocation( std::string local )
{
    m_Location = local;
}

void PacsParameters::setDescription( std::string desc )
{
    m_Desc = desc;
}

void PacsParameters::setDefault( std::string def )
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

std::string PacsParameters::getPacsAdr()
{
    return m_Adr;
}

std::string PacsParameters::getPacsPort()
{
    return m_Port;
}

std::string PacsParameters::getAELocal()
{
    return m_aeTitle;
}

std::string PacsParameters::getAEPacs()
{
    return m_aeCalled;
}

std::string PacsParameters::getLocalPort()
{
    return m_LocalPort;
}

std::string PacsParameters::getInstitution()
{
    return m_Inst;
}

std::string PacsParameters::getLocation()
{
    return m_Location;
}

std::string PacsParameters::getDescription()
{
    return m_Desc;
}

std::string PacsParameters::getDefault()
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

}

