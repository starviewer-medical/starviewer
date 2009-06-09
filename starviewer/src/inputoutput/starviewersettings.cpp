/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "starviewersettings.h"

#include <QDir>
#include <QApplication>
#include <QString>
#include <QHostInfo>

#include "logging.h"
#include "starviewerapplication.h"

const QString databaseRootKey("/cache/sdatabasePath" ); //indica on es troba la bd
const QString deleteOldStudiesHasNotViewedInDays("cache/deleteOldStudiesHasNotViewedInDays");
const QString deleteOldStudiesIfNotEnoughSpaceAvailable("cache/deleteOldStudiesIfNotEnoughSpaceAvailable");
const QString minimumSpaceRequiredToRetrieveInGbytesKey("/cache/minimumSpaceRequiredToRetrieveInGbytes" );
const QString GbytesOfOldStudiesToDeleteIfNotEnoughSapaceAvailable("/cache/GbytesOfOldStudiesToDeleteIfNotEnoughSapaceAvailable");
const QString cacheImagePathKey("/cache/imagePath" );
const QString AETitleMachineKey("/pacsparam/AETitle" );
const QString localPortKey("/pacsparam/localPort" );
const QString timeoutPacsKey("/pacsparam/timeout" );
const QString maxConnectionsKey("/pacsparam/MaxConnects" );
const QString MaximumDaysNotViewedStudy("/cache/MaximumDaysNotViewedStudy" );
const QString lastOpenedDICOMDIRPath("/interface/lastOpenedDICOMDIRPath"); // últim path des del que hem obert un dicomdir
const QString listenRisRequests("/risRequests/listen");
const QString listenRisRequestsPort("/risRequests/listenPort");
const QString viewAutomaticallyAStudyRetrievedFromRisRequest("/risRequests/viewAutomaticallyAStudyRetrievedFromRisRequest");

namespace udg {

StarviewerSettings::StarviewerSettings()
    :GroupSettingsName("PACS")
{
}

StarviewerSettings::~StarviewerSettings()
{
}

void StarviewerSettings::setDatabasePath( QString path )
{
    m_starviewerSettings.setValue( GroupSettingsName + databaseRootKey , QDir::fromNativeSeparators( path ) );
}

void StarviewerSettings::setCacheImagePath( QString path )
{
    m_starviewerSettings.setValue( GroupSettingsName + cacheImagePathKey , QDir::fromNativeSeparators( path ) );
}

void StarviewerSettings::setDeleteOldStudiesHasNotViewedInDays(bool deleteOldStudies)
{
    m_starviewerSettings.setValue(GroupSettingsName + deleteOldStudiesHasNotViewedInDays, deleteOldStudies);
}

void StarviewerSettings::setDeleteOldStudiesIfNotEnoughSpaceAvailable(bool deletedOldStudiesIfNotEnoughSpaceAvailable)
{
    m_starviewerSettings.setValue(GroupSettingsName + deleteOldStudiesIfNotEnoughSpaceAvailable, deletedOldStudiesIfNotEnoughSpaceAvailable);
}

void StarviewerSettings::setGbytesOfOldStudiesToDeleteIfNotEnoughSapaceAvailable(uint GbytesToDeleteOfStudies)
{
    m_starviewerSettings.setValue(GroupSettingsName + GbytesOfOldStudiesToDeleteIfNotEnoughSapaceAvailable, GbytesToDeleteOfStudies);
}

void StarviewerSettings::setMinimumSpaceRequiredToRetrieveInGbytes(uint minimumSpaceRequired)
{
    m_starviewerSettings.setValue(GroupSettingsName + minimumSpaceRequiredToRetrieveInGbytesKey, QString().setNum(minimumSpaceRequired));
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
    defaultDir = UserDataRootPath + "pacs/database/dicom.sdb";

    return QDir::toNativeSeparators( m_starviewerSettings.value( GroupSettingsName + databaseRootKey , defaultDir ).toString() );
}

bool StarviewerSettings::getDeleteOldStudiesHasNotViewedInDays()
{
    return m_starviewerSettings.value(GroupSettingsName + deleteOldStudiesHasNotViewedInDays, true).toBool();
}

bool StarviewerSettings::getDeleteOldStudiesIfNotEnoughSpaceAvailable()
{
    return m_starviewerSettings.value(GroupSettingsName + deleteOldStudiesIfNotEnoughSpaceAvailable, true).toBool();
}

uint StarviewerSettings::getGbytesOfOldStudiesToDeleteIfNotEnoughSapaceAvailable()
{
    return m_starviewerSettings.value(GroupSettingsName + GbytesOfOldStudiesToDeleteIfNotEnoughSapaceAvailable, 2).toUInt();
}

uint StarviewerSettings::getMinimumSpaceRequiredToRetrieveInGbytes()
{
    return m_starviewerSettings.value(GroupSettingsName + minimumSpaceRequiredToRetrieveInGbytesKey, "5").toString().toUInt();
}

uint StarviewerSettings::getMinimumSpaceRequiredToRetrieveInMbytes()
{
    return getMinimumSpaceRequiredToRetrieveInGbytes() * 1024;
}

QString StarviewerSettings::getCacheImagePath()
{
    QString defaultDir;
    QDir dir;

    //construim directori per defecte
    defaultDir = UserDataRootPath + "pacs/dicom/";

    return QDir::toNativeSeparators( m_starviewerSettings.value( GroupSettingsName + cacheImagePathKey , defaultDir ).toString() );
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
	return m_starviewerSettings.value( GroupSettingsName + AETitleMachineKey , QHostInfo::localHostName() ).toString(); 
}

QString StarviewerSettings::getTimeout()
{
    return m_starviewerSettings.value( GroupSettingsName + timeoutPacsKey , "20" ).toString();
}

QString StarviewerSettings::getLocalPort()
{
    return m_starviewerSettings.value( GroupSettingsName + localPortKey , "4006" ).toString();
}

QString StarviewerSettings::getMaxConnections()
{
    return m_starviewerSettings.value( GroupSettingsName + maxConnectionsKey , "3" ).toString();
}

QString StarviewerSettings::getLastOpenedDICOMDIRPath() const
{
	return m_starviewerSettings.value( GroupSettingsName + lastOpenedDICOMDIRPath, QDir::homePath() ).toString();
}

void StarviewerSettings::setLastOpenedDICOMDIRPath( QString const & path )
{
	m_starviewerSettings.setValue( GroupSettingsName + lastOpenedDICOMDIRPath, path );
}

/** Opcions de configuració d'escoltar peticions des del RIS*/

const QString viewAutomaticallyAStudyRetrievedFromRisRequest("/risRequests/viewAutomaticallyAStudyRetrievedFromRisRequest");


void StarviewerSettings::setListenRisRequests(bool listen)
{
    m_starviewerSettings.setValue(GroupSettingsName + listenRisRequests, listen);
}

bool StarviewerSettings::getListenRisRequests()
{
    return m_starviewerSettings.value(GroupSettingsName + listenRisRequests, true).toBool();
}

void StarviewerSettings::setListenPortRisRequests(int portListenRisRequests)
{
    m_starviewerSettings.setValue(GroupSettingsName + listenRisRequestsPort, portListenRisRequests); 
}

int StarviewerSettings::getListenPortRisRequests()
{
    return m_starviewerSettings.value(GroupSettingsName + listenRisRequestsPort, 11110).toInt();
}

void StarviewerSettings::setViewAutomaticallyAStudyRetrievedFromRisRequest(bool viewAutomaticallyStudy)
{
    m_starviewerSettings.setValue(GroupSettingsName + viewAutomaticallyAStudyRetrievedFromRisRequest, viewAutomaticallyStudy);
}

bool StarviewerSettings::getViewAutomaticallyAStudyRetrievedFromRisRequest()
{
    return m_starviewerSettings.value(GroupSettingsName + viewAutomaticallyAStudyRetrievedFromRisRequest, true).toBool();
}


};
