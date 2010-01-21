/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "pacsnetwork.h"

#include <QString>

#include <assoc.h>
#include "status.h"
#include "errordcmtk.h"

namespace udg {

PacsNetwork::PacsNetwork()
{
    m_networkQuery = NULL;
    m_networkRetrieve = NULL;
}

Status PacsNetwork::createNetworkRetrieve( int port , int timeout )
{
    Status state;
    OFCondition status;

    if ( m_networkRetrieve == NULL )
    {
        //Al fer inicialitzeNetwork s'obre el port local per acceptar connexions entrants DICOM
        status = ASC_initializeNetwork( NET_ACCEPTORREQUESTOR , port , timeout , &m_networkRetrieve );
        if ( !status.good() )
        {
            state.setStatus( status );
            return state;
        }
    }

    return state.setStatus( DcmtkNoError );
}

Status PacsNetwork::createNetworkQuery( int timeout )
{
    Status state;
    OFCondition status;

    if ( m_networkQuery == NULL )
    {
        status =ASC_initializeNetwork( NET_REQUESTOR , 0 , timeout , &m_networkQuery );
        if ( !status.good() )
        {
            state.setStatus( status );
            return state;
        }
    }

    return state.setStatus( DcmtkNoError );
}

T_ASC_Network * PacsNetwork::getNetworkQuery()
{
    return m_networkQuery;
}

T_ASC_Network * PacsNetwork::getNetworkRetrieve()
{
    return m_networkRetrieve;
}

void PacsNetwork::disconnect()
{
	OFCondition cond;

    //esborem la configuració de la xarxa per queries
    if ( m_networkQuery != NULL )
	{
		cond = ASC_dropNetwork( &m_networkQuery ); // delete net structure
		delete m_networkQuery;

		m_networkQuery = NULL;
	}

    //esborrem la configuració de la xarxa per retrieves

    if ( m_networkRetrieve != NULL )
	{
		cond = ASC_dropNetwork( &m_networkRetrieve );
		delete m_networkRetrieve;

		m_networkRetrieve = NULL;
	}
}

/** Destructor de la classe
  */
PacsNetwork::~PacsNetwork()
{
    disconnect();
}

};
