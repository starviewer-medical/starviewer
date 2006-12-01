/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 

#include <QDir>
#include <QApplication>

#include "starviewersettings.h"
#include "logging.h"

namespace udg {

StarviewerSettings::StarviewerSettings()
{
    m_starviewerSettings.beginGroup( "/Starviewer/" );
}

void StarviewerSettings::setDatabasePath( QString path )
{
    m_starviewerSettings.setValue( databaseRootKey , path );
}

void StarviewerSettings::setCacheImagePath( QString path )
{
    m_starviewerSettings.setValue( cacheImagePathKey , path );
}

void StarviewerSettings::setPoolSize(QString size )
{
    m_starviewerSettings.setValue( poolSizeKey , size );
}

void StarviewerSettings::setMaximumDaysNotViewedStudy( QString  days )
{
    m_starviewerSettings.setValue( MaximumDaysNotViewedStudy  ,  days );
}

QString StarviewerSettings::getDatabasePath()
{
    QDir dir;
    QString defaultDir;
    
    //construim directori per defecte
    defaultDir.append( dir.homePath() + "/.starviewer/pacs/database/dicom.sdb" );
    
    DEBUG_LOG(defaultDir.toAscii().constData());
    
    return m_starviewerSettings.value( databaseRootKey , defaultDir ).toString();
}

QString StarviewerSettings::getPoolSize()
{
    return m_starviewerSettings.value( poolSizeKey , "30" ).toString();
}

QString StarviewerSettings::getCacheImagePath()
{
    QString defaultDir;
    QDir dir;
    
    //construim directori per defecte
    defaultDir.append( dir.homePath() + "/.starviewer/pacs/dicom/" );
    
    return m_starviewerSettings.value( cacheImagePathKey , defaultDir ).toString();
}

QString StarviewerSettings::getMaximumDaysNotViewedStudy()
{
    return m_starviewerSettings.value( MaximumDaysNotViewedStudy  ,  "10" ).toString();
}

/************************ CONFIGURACIO PACS************************************************/

void StarviewerSettings::setAETitleMachine( QString AETitle )
{
    m_starviewerSettings.setValue( AETitleMachineKey , AETitle );
}

void StarviewerSettings::setTimeout( QString time )
{
    m_starviewerSettings.setValue( timeoutPacsKey , time );
}

void StarviewerSettings::setLocalPort( QString port )
{
    m_starviewerSettings.setValue( localPortKey , port );
}

void StarviewerSettings::setLanguage( QString lang )
{
    m_starviewerSettings.setValue( selectLanguageKey , lang );
}

void StarviewerSettings::setMaxConnections( QString maxConn )
{
    m_starviewerSettings.setValue( maxConnectionsKey , maxConn );
}

QString StarviewerSettings::getAETitleMachine()
{
    return m_starviewerSettings.value( AETitleMachineKey , "PACS" ).toString();
}

QString StarviewerSettings::getTimeout()
{
    return m_starviewerSettings.value( timeoutPacsKey , "20000" ).toString();
}

QString StarviewerSettings::getLocalPort()
{
    return m_starviewerSettings.value( localPortKey , "4006" ).toString();
}

QString StarviewerSettings::getLanguage()
{
    return m_starviewerSettings.value( selectLanguageKey , "104" ).toString();
}

QString StarviewerSettings::getMaxConnections()
{
    return m_starviewerSettings.value( maxConnectionsKey , "3" ).toString();
}

/************************ INTERFICIE ************************************************/
void StarviewerSettings::setStudyPacsListColumnWidth( int col , int width )
{
    QString key , strCol;
    
    strCol.setNum( col , 10 );
    key.insert( 0  , pacsColumnWidthKey );
    key.append( strCol );
    
    m_starviewerSettings.setValue( key , width );
}

void StarviewerSettings::setStudyCacheListColumnWidth( int col , int width )
{
    QString key , strCol;
    
    strCol.setNum( col , 10 );
    key.insert( 0  , cacheColumnWidthKey );
    key.append( strCol );
    
    m_starviewerSettings.setValue( key , width );
}

void StarviewerSettings::setStudyDicomdirListColumnWidth( int col , int width )
{
    QString key , strCol;
    
    strCol.setNum( col , 10 );
    key.insert( 0  , dicomdirColumnWidthKey );
    key.append( strCol );
    
    m_starviewerSettings.setValue( key , width );
}

int StarviewerSettings::getStudyPacsListColumnWidth( int col )
{   
    QString key , strCol;
    
    strCol.setNum( col , 10 );
    key.insert( 0 , pacsColumnWidthKey );
    key.append( strCol );
    
    return m_starviewerSettings.value( key , 100) .toInt();
}

int StarviewerSettings::getStudyCacheListColumnWidth( int col )
{   
    QString key , strCol;
    
    strCol.setNum( col , 10 );
    key.insert( 0 , cacheColumnWidthKey );
    key.append( strCol );
    
    return m_starviewerSettings.value( key , 100 ).toInt();
}

int StarviewerSettings::getStudyDicomdirListColumnWidth( int col )
{   
    QString key , strCol;
    
    strCol.setNum( col , 10 );
    key.insert( 0 , dicomdirColumnWidthKey );
    key.append( strCol );
    
    return m_starviewerSettings.value( key , 100 ).toInt();
}

/*** Dades de la institució*/

void StarviewerSettings::setInstitutionName( QString institutionNameString )
{
    m_starviewerSettings.setValue( InstitutionName , institutionNameString );
}

void StarviewerSettings::setInstitutionAddress ( QString institutionAddressString )
{
    m_starviewerSettings.setValue( InstitutionAddress , institutionAddressString );
}

void StarviewerSettings::setInstitutionTown( QString institutionTownString )
{
    m_starviewerSettings.setValue( InstitutionTown , institutionTownString );
}

void StarviewerSettings::setInstitutionZipCode( QString institutionZipCodeString )
{
    m_starviewerSettings.setValue( InstitutionZipCode , institutionZipCodeString );
}

void StarviewerSettings::setInstitutionCountry( QString institutionCountryString )
{
    m_starviewerSettings.setValue( InstitutionCountry , institutionCountryString );
}

void StarviewerSettings::setInstitutionPhoneNumber( QString institutionPhoneNumberString )
{
    m_starviewerSettings.setValue( InstitutionPhoneNumber , institutionPhoneNumberString );
}

void StarviewerSettings::setInstitutionEmail( QString institutionEmailString )
{
    m_starviewerSettings.setValue( InstitutionEmail , institutionEmailString );
}

QString StarviewerSettings::getInstitutionName()
{
    return m_starviewerSettings.value( InstitutionName , "" ).toString();
}

QString StarviewerSettings::getInstitutionAddress()
{
    return m_starviewerSettings.value( InstitutionAddress , "" ).toString();
}

QString StarviewerSettings::getInstitutionTown()
{
    return m_starviewerSettings.value( InstitutionTown , "" ).toString();
}

QString StarviewerSettings::getInstitutionCountry()
{
    return m_starviewerSettings.value( InstitutionCountry , "" ).toString();
}

QString StarviewerSettings::getInstitutionZipCode()
{
    return m_starviewerSettings.value( InstitutionZipCode , "" ).toString();
}

QString StarviewerSettings::getInstitutionPhoneNumber()
{
    return m_starviewerSettings.value( InstitutionPhoneNumber , "" ).toString();
}

QString StarviewerSettings::getInstitutionEmail()
{
    return m_starviewerSettings.value( InstitutionEmail , "" ).toString();
}

StarviewerSettings::~StarviewerSettings()
{
}

};
