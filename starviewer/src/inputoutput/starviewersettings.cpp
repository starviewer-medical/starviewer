/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#include "starviewersettings.h"
#include <stream.h>
#include <QDir>
#include <QApplication>

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
    QDir currentDir;
    QString defaultDir;
    
    //construim directori per defecte
    defaultDir = QApplication::applicationDirPath(); //directori actual
    defaultDir.append( "/pacscache/database/dicom.sdb" );
    
    return m_starviewerSettings.value( databaseRootKey , defaultDir ).toString();
}

QString StarviewerSettings::getPoolSize()
{
    return m_starviewerSettings.value( poolSizeKey , "30" ).toString();
}

QString StarviewerSettings::getCacheImagePath()
{
    QString defaultDir;
    
    //construim directori per defecte
    defaultDir = QApplication::applicationDirPath(); //directori actual
    defaultDir.append( "/pacscache/dicom/" );
    
    return m_starviewerSettings.value( cacheImagePathKey , defaultDir ).toString();
}

QString StarviewerSettings::getMaximumDaysNotViewedStudy()
{
    return m_starviewerSettings.value( MaximumDaysNotViewedStudy  ,  "15" ).toString();
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
    return m_starviewerSettings.value( localPortKey , "104" ).toString();
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

StarviewerSettings::~StarviewerSettings()
{
}

};
