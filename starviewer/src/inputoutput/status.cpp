/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include <string>
 
#include "status.h"

namespace udg{

Status::Status()
{
    m_numberError=-1;
}

bool Status:: good()
{    
	return m_success;
}

std::string Status:: text()
{
    return m_descText;
}

int Status:: code()
{
	return m_numberError;
}

Status Status:: setStatus( const OFCondition  status )
{
	m_descText = status.text();
	m_success = status.good();
	m_numberError = status.code();	
 
    return *this;
}

Status Status:: setStatus( std::string desc , bool ok , int numError )
{
    m_descText = desc;
    m_success = ok;
    m_numberError = numError;
    
    return *this;
}

};
