/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "status.h"

#include <ofcond.h> //provide the OFcondition structure and his members

namespace udg{

Status::Status()
{
    m_numberError=-1;
}

bool Status::good() const
{
	return m_success;
}

QString Status::text() const
{
    return m_descText;
}

int Status::code() const
{
	return m_numberError;
}

Status Status::setStatus( const OFCondition status )
{
	m_descText = status.text();
	m_success = status.good();
	m_numberError = status.code();
    
    switch( m_numberError )
    {
        case 0:
            m_errorType = NoError;
        break;
            
        case 1000:
            m_errorType = UnknowError;
        break;
            
        case 1120:
            m_errorType = NoConnectionError;
        break;
        
        case 1121:
            m_errorType = NoMaskError;
        break;
            
        case 1127:
            m_errorType = MaskInsertTagError;
        break;
            
        case 1200:
            m_errorType = CanNotConnectError;
        break;
        
        case 1201:
            m_errorType = UserCancellation;
        break;
    }

    return *this;
}

Status Status::setStatus( QString desc , bool ok , int numError )
{
    m_descText = desc;
    m_success = ok;
    m_numberError = numError;

    return *this;
}

};
