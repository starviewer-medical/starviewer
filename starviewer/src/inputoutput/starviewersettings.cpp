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
#include <QDebug>

namespace udg {

StarviewerSettings::StarviewerSettings()
    :GroupSettingsName("PACS")
{
}

void StarviewerSettings::setDatabasePath( QString path )
{
    m_starviewerSettings.setValue( GroupSettingsName + databaseRootKey , path );
}

void StarviewerSettings::setCacheImagePath( QString path )
{
    m_starviewerSettings.setValue( GroupSettingsName + cacheImagePathKey , path );
}

void StarviewerSettings::setPoolSize(QString size )
{
    m_starviewerSettings.setValue( GroupSettingsName + poolSizeKey , size );
}

void StarviewerSettings::setMaximumDaysNotViewedStudy( QString  days )
{
    m_starviewerSettings.setValue( GroupSettingsName + MaximumDaysNotViewedStudy  ,  days );
}

QString StarviewerSettings::getDatabasePath()
{
    QDir dir;
    QString defaultDir;
    
    //construim directori per defecte
    defaultDir.append( dir.homePath() + "/.starviewer/pacs/database/dicom.sdb" );
    
    return m_starviewerSettings.value( GroupSettingsName + databaseRootKey , defaultDir ).toString();
}

QString StarviewerSettings::getPoolSize()
{
    return m_starviewerSettings.value( GroupSettingsName + poolSizeKey , "20" ).toString();
}

QString StarviewerSettings::getCacheImagePath()
{
    QString defaultDir;
    QDir dir;
    
    //construim directori per defecte
    defaultDir.append( dir.homePath() + "/.starviewer/pacs/dicom/" );
    
    return m_starviewerSettings.value( GroupSettingsName + cacheImagePathKey , defaultDir ).toString();
}

QString StarviewerSettings::getMaximumDaysNotViewedStudy()
{
    return m_starviewerSettings.value( GroupSettingsName + MaximumDaysNotViewedStudy  ,  "7" ).toString();
}

/************************ CONFIGURACIO PACS************************************************/

void StarviewerSettings::setAETitleMachine( QString AETitle )
{
    m_starviewerSettings.setValue( GroupSettingsName + AETitleMachineKey , AETitle );
}

void StarviewerSettings::setTimeout( QString time )
{
    m_starviewerSettings.setValue( GroupSettingsName + timeoutPacsKey , time );
}

void StarviewerSettings::setLocalPort( QString port )
{
    m_starviewerSettings.setValue( GroupSettingsName + localPortKey , port );
}

void StarviewerSettings::setMaxConnections( QString maxConn )
{
    m_starviewerSettings.setValue( GroupSettingsName + maxConnectionsKey , maxConn );
}

QString StarviewerSettings::getAETitleMachine()
{
    return m_starviewerSettings.value( GroupSettingsName + AETitleMachineKey , GroupSettingsName ).toString();
}

QString StarviewerSettings::getTimeout()
{
    return m_starviewerSettings.value( GroupSettingsName + timeoutPacsKey , "20000" ).toString();
}

QString StarviewerSettings::getLocalPort()
{
    return m_starviewerSettings.value( GroupSettingsName + localPortKey , "4006" ).toString();
}

QString StarviewerSettings::getMaxConnections()
{
    return m_starviewerSettings.value( GroupSettingsName + maxConnectionsKey , "3" ).toString();
}

/************************ INTERFICIE ************************************************/
void StarviewerSettings::setStudyPacsListColumnWidth( int col , int width )
{
    QString key = GroupSettingsName + pacsColumnWidthKey + QString::number( col );
    m_starviewerSettings.setValue( key , width );
}

void StarviewerSettings::setStudyCacheListColumnWidth( int col , int width )
{
    QString key = GroupSettingsName + cacheColumnWidthKey + QString::number( col );
    m_starviewerSettings.setValue( key , width );
}

void StarviewerSettings::setStudyDicomdirListColumnWidth( int col , int width )
{
    QString key = GroupSettingsName + dicomdirColumnWidthKey + QString::number( col );
    m_starviewerSettings.setValue( key , width );
}

int StarviewerSettings::getStudyPacsListColumnWidth( int col )
{
    QString key = GroupSettingsName + pacsColumnWidthKey + QString::number(col);
    return m_starviewerSettings.value( key , 100) .toInt();
}

int StarviewerSettings::getStudyCacheListColumnWidth( int col )
{
    QString key = GroupSettingsName + cacheColumnWidthKey + QString::number( col );
    return m_starviewerSettings.value( key , 100 ).toInt();
}

int StarviewerSettings::getStudyDicomdirListColumnWidth( int col )
{
    QString key = GroupSettingsName + dicomdirColumnWidthKey + QString::number( col );
    return m_starviewerSettings.value( key , 100 ).toInt();
}

/*** Dades de la institució*/

void StarviewerSettings::setInstitutionName( QString institutionNameString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionName , institutionNameString );
}

void StarviewerSettings::setInstitutionAddress ( QString institutionAddressString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionAddress , institutionAddressString );
}

void StarviewerSettings::setInstitutionTown( QString institutionTownString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionTown , institutionTownString );
}

void StarviewerSettings::setInstitutionZipCode( QString institutionZipCodeString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionZipCode , institutionZipCodeString );
}

void StarviewerSettings::setInstitutionCountry( QString institutionCountryString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionCountry , institutionCountryString );
}

void StarviewerSettings::setInstitutionPhoneNumber( QString institutionPhoneNumberString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionPhoneNumber , institutionPhoneNumberString );
}

void StarviewerSettings::setInstitutionEmail( QString institutionEmailString )
{
    m_starviewerSettings.setValue( GroupSettingsName + InstitutionEmail , institutionEmailString );
}

QString StarviewerSettings::getInstitutionName()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionName , "" ).toString();
}

QString StarviewerSettings::getInstitutionAddress()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionAddress , "" ).toString();
}

QString StarviewerSettings::getInstitutionTown()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionTown , "" ).toString();
}

QString StarviewerSettings::getInstitutionCountry()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionCountry , "" ).toString();
}

QString StarviewerSettings::getInstitutionZipCode()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionZipCode , "" ).toString();
}

QString StarviewerSettings::getInstitutionPhoneNumber()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionPhoneNumber , "" ).toString();
}

QString StarviewerSettings::getInstitutionEmail()
{
    return m_starviewerSettings.value( GroupSettingsName + InstitutionEmail , "" ).toString();
}

StarviewerSettings::~StarviewerSettings()
{
}

};
