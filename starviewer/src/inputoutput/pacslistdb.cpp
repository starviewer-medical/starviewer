/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "pacslistdb.h"

#include <QString>

#include <sqlite3.h>

#include "pacslist.h"
#include "databaseconnection.h"
#include "status.h"
#include "pacsparameters.h"
#include "logging.h"
#include "errordcmtk.h"

namespace udg {

PacsListDB::PacsListDB(): m_arrayQSettingsName( "PacsList" )
{
}

Status PacsListDB::insertPacs( PacsParameters *pacs )
{
    Status state;

    state.setStatus( DcmtkNoError );

    if ( !isPacsDeleted( pacs ) )
    {
        if ( !existPacs( pacs ) )
        {
            int arrayIndex = countPacsParamentersInQSettings();//busquem a quina posició hem de gravar el següent pacs
        
            pacs->setPacsID( arrayIndex );
            pacs->setIsDeleted( false );
        
            setPacsParametersToQSettingsValues( pacs, arrayIndex, countPacsParamentersInQSettings() + 1 ); 
        }
        else state.setStatus( "El pacs ja existeix ", false, 2099 );
    }
    else //El pacs està donat de baixa el tornem a donar d'alta
    {
        PacsParameters *pacsDeleted = new PacsParameters();

        queryPacs( pacsDeleted, pacs->getAEPacs() );

        pacs->setPacsID( pacsDeleted->getPacsID() );

        setPacsParametersToQSettingsValues( pacs, pacsDeleted->getPacsID(), countPacsParamentersInQSettings() ); 
    }

    return state;
}

Status PacsListDB::updatePacs( PacsParameters *pacsToUpdate )
{
    Status state;

	setPacsParametersToQSettingsValues( pacsToUpdate, pacsToUpdate->getPacsID(), countPacsParamentersInQSettings() );

    return state.setStatus( DcmtkNoError );
}

Status PacsListDB::queryPacsList( PacsList &list )
{
    Status state;

    for ( int arrayIndex = 0 ; arrayIndex < countPacsParamentersInQSettings() ; arrayIndex ++ )
    {
        PacsParameters pacs = getPacsParametersFromQSettinsValues( arrayIndex );

        if (!pacs.isDeleted()) list.insertPacs( pacs );
    }

    return state.setStatus( DcmtkNoError );
}

Status PacsListDB::queryPacs( PacsParameters *pacs, QString AETitle )
{
    Status state;
    int arrayIndex = 0;
    bool trobat = false;
    PacsParameters pacsFromQSettings; 

    while ( arrayIndex < countPacsParamentersInQSettings()  && !trobat )
    {
        pacsFromQSettings = getPacsParametersFromQSettinsValues( arrayIndex );

        if ( pacsFromQSettings.getAEPacs() == AETitle )
        {
            trobat = true;
        }
        else arrayIndex++;
    }

    if ( trobat )
    {
        pacs->setAEPacs( pacsFromQSettings.getAEPacs() );
        pacs->setDefault( pacsFromQSettings.getDefault() );
        pacs->setDescription( pacsFromQSettings.getDescription() );
        pacs->setInstitution( pacsFromQSettings.getInstitution() );
        pacs->setIsDeleted( pacsFromQSettings.isDeleted() );
        pacs->setPacsPort( pacsFromQSettings.getPacsPort() );
        pacs->setPacsID( pacsFromQSettings.getPacsID() );
        pacs->setPacsAdr( pacsFromQSettings.getPacsAdr() );
        pacs->setLocation( pacsFromQSettings.getLocation() );
    }

    return state.setStatus( DcmtkNoError );
}

Status PacsListDB::queryPacs( PacsParameters *pacs, int pacsID )
{
    Status state;
    int arrayIndex = 0;
    bool trobat = false;
    PacsParameters pacsFromQSettings; 

    while ( arrayIndex < countPacsParamentersInQSettings() && !trobat )
    {
        pacsFromQSettings = getPacsParametersFromQSettinsValues( arrayIndex );

        if ( pacsFromQSettings.getPacsID() == pacsID )
        {
            trobat = true;
        }
        else arrayIndex++;
    }

    if ( trobat )
    {
        pacs->setAEPacs( pacsFromQSettings.getAEPacs() );
        pacs->setDefault( pacsFromQSettings.getDefault() );
        pacs->setDescription( pacsFromQSettings.getDescription() );
        pacs->setInstitution( pacsFromQSettings.getInstitution() );
        pacs->setIsDeleted( pacsFromQSettings.isDeleted() );
        pacs->setPacsPort( pacsFromQSettings.getPacsPort() );
        pacs->setPacsID( pacsFromQSettings.getPacsID() );
        pacs->setPacsAdr( pacsFromQSettings.getPacsAdr() );
        pacs->setLocation( pacsFromQSettings.getLocation() );
    }

    return state.setStatus( DcmtkNoError );
}

bool PacsListDB::existPacs( PacsParameters *pacs )
{
    int arrayIndex = 0;
    bool trobat = false;
    PacsParameters pacsFromQSettings;

    while ( arrayIndex < countPacsParamentersInQSettings()  && !trobat )
    {
        pacsFromQSettings = getPacsParametersFromQSettinsValues( arrayIndex );

        if ( pacsFromQSettings.getAEPacs() == pacs->getAEPacs() )
        {
            trobat = true;
        }
        else arrayIndex++;
    }

    return trobat;
}

bool PacsListDB::isPacsDeleted( PacsParameters *pacs )
{
    int arrayIndex = 0;
    bool trobat = false;
    PacsParameters pacsFromQSettings;

    while ( arrayIndex < countPacsParamentersInQSettings()  && !trobat )
    {
        pacsFromQSettings = getPacsParametersFromQSettinsValues( arrayIndex );

        if ( pacsFromQSettings.getAEPacs() == pacs->getAEPacs() && pacsFromQSettings.isDeleted() )
        {
            trobat = true;
        }
        else arrayIndex++;
    }

    return trobat;
}

Status PacsListDB::deletePacs( int pacsID )
{
    Status state;
    PacsParameters *pacsToDelete = new PacsParameters;;

    queryPacs( pacsToDelete, pacsID );

    pacsToDelete->setIsDeleted( true );//el marquem com a esborrat

    setPacsParametersToQSettingsValues( pacsToDelete, pacsToDelete->getPacsID(), countPacsParamentersInQSettings() );

    return state.setStatus( DcmtkNoError );

}

void PacsListDB::setPacsParametersToQSettingsValues( PacsParameters *pacs, int arrayIndex, int sizeOfArray )
{
    /*Especifiquem  quina serà la mida de l'array de PacsParameters que guardem*/
    m_pacsListQSettings.beginWriteArray( m_arrayQSettingsName, sizeOfArray );

    m_pacsListQSettings.setArrayIndex( arrayIndex );
    m_pacsListQSettings.setValue( "ID", pacs->getPacsID() );
    m_pacsListQSettings.setValue( "AETitle", pacs->getAEPacs() );
    m_pacsListQSettings.setValue( "PacsPort", pacs->getPacsPort() );
    m_pacsListQSettings.setValue( "Location", pacs->getLocation() );
    m_pacsListQSettings.setValue( "Institution", pacs->getInstitution() );
    m_pacsListQSettings.setValue( "Default", pacs->getDefault() );
    m_pacsListQSettings.setValue( "PacsHostname", pacs->getPacsAdr() );
    m_pacsListQSettings.setValue( "Deleted", pacs->isDeleted() );
    m_pacsListQSettings.setValue( "Description", pacs->getDescription() );

    m_pacsListQSettings.endArray();
}

PacsParameters PacsListDB::getPacsParametersFromQSettinsValues( int arrayIndex )
{
    PacsParameters returnPacsParameters;

    m_pacsListQSettings.beginReadArray( m_arrayQSettingsName );

    m_pacsListQSettings.setArrayIndex( arrayIndex );

    returnPacsParameters.setPacsID( m_pacsListQSettings.value( "ID" ).toInt() );
    returnPacsParameters.setAEPacs( m_pacsListQSettings.value( "AETitle" ).toString()  );
    returnPacsParameters.setPacsPort( m_pacsListQSettings.value( "PacsPort" ).toString() );
    returnPacsParameters.setLocation( m_pacsListQSettings.value( "Location" ).toString()  );
    returnPacsParameters.setInstitution( m_pacsListQSettings.value( "Institution" ).toString() );
    returnPacsParameters.setDefault( m_pacsListQSettings.value( "Default" ).toString() );
    returnPacsParameters.setPacsAdr( m_pacsListQSettings.value( "PacsHostname" ).toString() );
    returnPacsParameters.setIsDeleted( m_pacsListQSettings.value( "Deleted" ).toBool() );
    returnPacsParameters.setDescription( m_pacsListQSettings.value( "Description" ).toString() );

    m_pacsListQSettings.endArray();

    return returnPacsParameters;
}

int PacsListDB::countPacsParamentersInQSettings()
{
    int arrayIndex = m_pacsListQSettings.beginReadArray( m_arrayQSettingsName );
    m_pacsListQSettings.endArray();

    return arrayIndex;
}

PacsListDB::~PacsListDB()
{
}

};
