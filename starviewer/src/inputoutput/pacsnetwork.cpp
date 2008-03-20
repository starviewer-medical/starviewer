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

PacsNetwork* PacsNetwork::pacsNetwork = 0;

PacsNetwork* PacsNetwork::getPacsNetwork()
{
    if ( pacsNetwork == 0 )
    {
        pacsNetwork = new PacsNetwork;
    }

    return pacsNetwork;
}

Status PacsNetwork::createNetworkRetrieve( int port , int timeout )
{
    Status state;
    OFCondition status;

    if ( m_networkRetrieve == NULL )
    {
        status = ASC_initializeNetwork( NET_ACCEPTORREQUESTOR , port , timeout , &m_networkRetrieve );
        if ( !status.good() )
        {
            state.setStatus( status );
            return state;
        }
    }

    return state.setStatus( CORRECT );
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

    return state.setStatus( CORRECT );
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
    //esborem la configuració de la xarxa per queries
    if ( m_networkQuery != NULL )  ASC_dropNetwork( &m_networkQuery ); // delete net structure

    //esborrem la configuració de la xarxa per retrieves

    if ( m_networkRetrieve != NULL ) ASC_dropNetwork( &m_networkRetrieve );
}

/** Destructor de la classe
  */
PacsNetwork::~PacsNetwork()
{
    disconnect();
}

};
