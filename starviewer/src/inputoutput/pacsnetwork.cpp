/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "pacsnetwork.h"

namespace udg {

/** Constructor de la classe
  */
PacsNetwork::PacsNetwork()
{
    m_networkQuery = NULL;
    m_networkRetrieve = NULL;
    
}

PacsNetwork* PacsNetwork::pacsNetwork = 0;

/** Retorna una instancia de l'objecte
  *        @return instancia de l'objecte
  */
PacsNetwork* PacsNetwork::getPacsNetwork()
{
    if (pacsNetwork == 0)
    {
        pacsNetwork = new PacsNetwork;
    }
    
    return pacsNetwork; 
}

/** Preparar un network per a fer l'acció de retrieve imatges, si el network ja existeix, els parametres de timeout i port s'ignore
  * i retorna el creat anterioment
  *            @param port a utilitzar per rebre les images
  *            @param timeout de l'accio retrieve
  */
Status PacsNetwork::createNetworkRetrieve(int port,int timeout)
{
    Status state;
    OFCondition status;
    
    if (m_networkRetrieve == NULL)
    {
        status =ASC_initializeNetwork(NET_ACCEPTORREQUESTOR, port, timeout, &m_networkRetrieve);
        if (!status.good()) 
        {
            state.setStatus(status);
            return state;
        }
    }
    
    return state.setStatus(CORRECT);
}

/** Preparar un network per a fer l'acció de query, si el network ja existeix, els parametres de timeout i port s'ignoren
  * i retorna el creat anterioment
  *            @param timeout dels query
  */
Status PacsNetwork::createNetworkQuery(int timeout)
{
    Status state;
    OFCondition status;
    
    if (m_networkQuery == NULL)
    {
        status =ASC_initializeNetwork(NET_REQUESTOR, 0, timeout, &m_networkQuery);
        if (!status.good()) 
        {
            state.setStatus(status);
            return state;
        }
    }
    
    return state.setStatus(CORRECT);
}

/** retorna la configuració network de per fer queries
                @return Retorna el network per fer queries
  */
T_ASC_Network * PacsNetwork::getNetworkQuery()
{
    return m_networkQuery;
}

/** retorna la configuració network de per fer els retrieves
                @return Retorna el network per fer els retrieves
  */
T_ASC_Network * PacsNetwork::getNetworkRetrieve()
{
    return m_networkRetrieve;
}

/** Desconnecta els network
  */
void PacsNetwork::disconnect()
{
    //esborem la configuració de la xarxa per queries
    if (m_networkQuery != NULL)  ASC_dropNetwork(&m_networkQuery); // delete net structure

    //esborrem la configuració de la xarxa per retrieves
    
    if (m_networkRetrieve != NULL) ASC_dropNetwork(&m_networkRetrieve);
}

/** Destructor de la classe
  */
PacsNetwork::~PacsNetwork()
{
    disconnect();
}


};
