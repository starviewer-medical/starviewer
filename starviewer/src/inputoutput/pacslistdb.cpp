/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "pacslistdb.h"

#include <QString>

#include "pacsparameters.h"
#include "logging.h"

namespace udg {

const QString PacsListDB::PacsListConfigurationSectionName = "PacsList";

PacsListDB::PacsListDB()
{
}

bool PacsListDB::insertPacs(const PacsParameters &pacs)
{
    if ( !isPacsDeleted( pacs ) )
    {
        if ( !existPacs( pacs ) )
        {
            int arrayIndex = countPacsParamentersInQSettings();//busquem a quina posició hem de gravar el següent pacs

            PacsParameters pacsNew = pacs;
            pacsNew.setPacsID( arrayIndex );
            pacsNew.setIsDeleted( false );

            setPacsParametersToQSettingsValues( pacsNew, arrayIndex, countPacsParamentersInQSettings() + 1 );
        }
        else
        {
            return false;
        }
    }
    else //El pacs està donat de baixa el tornem a donar d'alta
    {
        PacsParameters pacsDeleted = queryPacs( pacs.getAEPacs() );

        PacsParameters pacsNew = pacs;
        pacsNew.setPacsID( pacsDeleted.getPacsID() );

        setPacsParametersToQSettingsValues( pacsNew, pacsDeleted.getPacsID(), countPacsParamentersInQSettings() );
    }

    return true;
}

void PacsListDB::updatePacs(const PacsParameters &pacsToUpdate)
{
    setPacsParametersToQSettingsValues( pacsToUpdate, pacsToUpdate.getPacsID(), countPacsParamentersInQSettings() );
}

QList<PacsParameters> PacsListDB::queryPacsList()
{
    QList<PacsParameters> outResultsPacslist;
    for ( int arrayIndex = 0 ; arrayIndex < countPacsParamentersInQSettings() ; arrayIndex ++ )
    {
        PacsParameters pacs = getPacsParametersFromQSettinsValues( arrayIndex );

        if (!pacs.isDeleted()) outResultsPacslist.append( pacs );
    }

    return outResultsPacslist;
}

PacsParameters PacsListDB::queryPacs(QString AETitle)
{
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

    PacsParameters pacs;
    if ( trobat )
    {
        pacs.setAEPacs( pacsFromQSettings.getAEPacs() );
        pacs.setDefault( pacsFromQSettings.getDefault() );
        pacs.setDescription( pacsFromQSettings.getDescription() );
        pacs.setInstitution( pacsFromQSettings.getInstitution() );
        pacs.setIsDeleted( pacsFromQSettings.isDeleted() );
        pacs.setPacsPort( pacsFromQSettings.getPacsPort() );
        pacs.setPacsID( pacsFromQSettings.getPacsID() );
        pacs.setPacsAdr( pacsFromQSettings.getPacsAdr() );
        pacs.setLocation( pacsFromQSettings.getLocation() );
    }

    return pacs;
}

PacsParameters PacsListDB::queryPacs(int pacsID)
{
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

    PacsParameters pacs;
    if ( trobat )
    {
        pacs.setAEPacs( pacsFromQSettings.getAEPacs() );
        pacs.setDefault( pacsFromQSettings.getDefault() );
        pacs.setDescription( pacsFromQSettings.getDescription() );
        pacs.setInstitution( pacsFromQSettings.getInstitution() );
        pacs.setIsDeleted( pacsFromQSettings.isDeleted() );
        pacs.setPacsPort( pacsFromQSettings.getPacsPort() );
        pacs.setPacsID( pacsFromQSettings.getPacsID() );
        pacs.setPacsAdr( pacsFromQSettings.getPacsAdr() );
        pacs.setLocation( pacsFromQSettings.getLocation() );
    }

    return pacs;
}

bool PacsListDB::existPacs(const PacsParameters &pacs)
{
    int arrayIndex = 0;
    bool trobat = false;
    PacsParameters pacsFromQSettings;

    while ( arrayIndex < countPacsParamentersInQSettings()  && !trobat )
    {
        pacsFromQSettings = getPacsParametersFromQSettinsValues( arrayIndex );

        if ( pacsFromQSettings.getAEPacs() == pacs.getAEPacs() )
        {
            trobat = true;
        }
        else arrayIndex++;
    }

    return trobat;
}

bool PacsListDB::isPacsDeleted(const PacsParameters &pacs)
{
    int arrayIndex = 0;
    bool trobat = false;
    PacsParameters pacsFromQSettings;

    while ( arrayIndex < countPacsParamentersInQSettings()  && !trobat )
    {
        pacsFromQSettings = getPacsParametersFromQSettinsValues( arrayIndex );

        if ( pacsFromQSettings.getAEPacs() == pacs.getAEPacs() && pacsFromQSettings.isDeleted() )
        {
            trobat = true;
        }
        else arrayIndex++;
    }

    return trobat;
}

void PacsListDB::deletePacs( int pacsID )
{
    PacsParameters pacsToDelete = queryPacs(pacsID);

    pacsToDelete.setIsDeleted( true );//el marquem com a esborrat

    setPacsParametersToQSettingsValues( pacsToDelete, pacsToDelete.getPacsID(), countPacsParamentersInQSettings() );
}

void PacsListDB::setPacsParametersToQSettingsValues(const PacsParameters &pacs, int arrayIndex, int sizeOfArray)
{
    /*Especifiquem  quina serà la mida de l'array de PacsParameters que guardem*/
    m_pacsListQSettings.beginWriteArray(PacsListConfigurationSectionName, sizeOfArray);

    m_pacsListQSettings.setArrayIndex( arrayIndex );
    m_pacsListQSettings.setValue( "ID", pacs.getPacsID() );
    m_pacsListQSettings.setValue( "AETitle", pacs.getAEPacs() );
    m_pacsListQSettings.setValue( "PacsPort", pacs.getPacsPort() );
    m_pacsListQSettings.setValue( "Location", pacs.getLocation() );
    m_pacsListQSettings.setValue( "Institution", pacs.getInstitution() );
    m_pacsListQSettings.setValue( "Default", pacs.getDefault() );
    m_pacsListQSettings.setValue( "PacsHostname", pacs.getPacsAdr() );
    m_pacsListQSettings.setValue( "Deleted", pacs.isDeleted() );
    m_pacsListQSettings.setValue( "Description", pacs.getDescription() );

    m_pacsListQSettings.endArray();
}

PacsParameters PacsListDB::getPacsParametersFromQSettinsValues( int arrayIndex )
{
    PacsParameters returnPacsParameters;

    m_pacsListQSettings.beginReadArray(PacsListConfigurationSectionName);

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
    int arrayIndex = m_pacsListQSettings.beginReadArray(PacsListConfigurationSectionName);
    m_pacsListQSettings.endArray();

    return arrayIndex;
}

PacsListDB::~PacsListDB()
{
}

};
